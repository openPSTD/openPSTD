//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date: 18-7-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Viewer2D.h"
#include <qopenglcontext.h>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QPainter>
#include <algorithm>
#include <math.h>
#include <string>
#include <memory>
#include "layers/GridLayer.h"
#include "layers/SceneLayer.h"
#include "layers/IconLayer.h"
#include "layers/InteractiveLayer.h"
#include "layers/ResultsLayer.h"
#include <boost/lexical_cast.hpp>
#include <QtGui/QPainter>
#include "operations/ViewOperations.h"
#include "operations/LambdaOperation.h"
#include "mouse/MouseStrategy.h"

namespace OpenPSTD
{
    namespace GUI
    {

        void GLError(std::string name)
        {
            GLenum err = glGetError();
            if (err != GL_NO_ERROR)
            {
                std::cout << "------------------------------------------------------" << std::endl;
                std::cout << "=" << name << std::endl;

                while (err != GL_NO_ERROR)
                {
                    std::cout << err << ": " << gluErrorString(err) << std::endl;
                    err = glGetError();
                }
            }
        }

        void DeleteNothing(void *ptr)
        {

        }

        void DeleteTexture(void *ptr)
        {
            if (ptr != nullptr)
                ((QOpenGLTexture *) ptr)->destroy();
        }

        Viewer2D::Viewer2D(QWidget *parent)
                : QOpenGLWidget(parent), layers()
        {
            this->layers.push_back(std::make_shared<ResultsLayer>());
            this->layers.push_back(std::make_shared<GridLayer>());
            this->layers.push_back(std::make_shared<SceneLayer>());
            this->layers.push_back(std::make_shared<IconLayer>());
            this->layers.push_back(std::make_shared<InteractiveLayer>());

            QSurfaceFormat format;
            format.setProfile(QSurfaceFormat::CoreProfile);
            format.setVersion(3, 3);
            format.setSamples(4);
            this->setFormat(format);
        }

        void Viewer2D::SetOperationRunner(std::weak_ptr<OperationRunner> operationRunner)
        {
            this->operationRunner = operationRunner;
        }


        void Viewer2D::paintGL()
        {
            if(this->loaded)
            {
                std::unique_ptr<QOpenGLFunctions, void (*)(void *)> f(QOpenGLContext::currentContext()->functions(),
                                                                      DeleteNothing);

                for (int i = 0; i < this->layers.size(); i++)
                {
                    this->layers[i]->PaintGLVisibilityCheck(this, f);
                    GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->PaintGL");
                }
                f->glActiveTexture(GL_TEXTURE0);
            }


        }

        void Viewer2D::initializeGL()
        {
            std::unique_ptr<QOpenGLFunctions, void (*)(void *)> f(QOpenGLContext::currentContext()->functions(),
                                                                  DeleteNothing);

            const GLubyte * vendor = f->glGetString(GL_VENDOR);
            const GLubyte * renderer = f->glGetString(GL_RENDERER);
            const GLubyte * version = f->glGetString(GL_VERSION);
            const GLubyte * shadingLanguageVersion = f->glGetString(GL_SHADING_LANGUAGE_VERSION);
            int major, minor;
            f->glGetIntegerv(GL_MAJOR_VERSION, &major);
            f->glGetIntegerv(GL_MINOR_VERSION, &minor);

            std::cout << "Vendor: " << vendor << std::endl;
            std::cout << "Renderer: " << renderer << std::endl;
            std::cout << "Version: " << version << std::endl;
            std::cout << "Shading language version: " << shadingLanguageVersion << std::endl;
            std::cout << "OpenGL version: " << major << "." << minor << std::endl;

            f->glClearColor(0, 0, 0, 1);

            f->glDisable(GL_CULL_FACE);
            GLError("Viewer2D:: f->glDisable(GL_CULL_FACE)");
            f->glEnable(GL_BLEND);
            GLError("Viewer2D:: f->glEnable(GL_BLEND)");
            f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GLError("Viewer2D:: F->glBlendFunc");

            _vao = new QOpenGLVertexArrayObject( this );
            _vao->create();
            _vao->bind();


            textRenderer = std::make_shared<TextRenderer>(f);

            for (int i = 0; i < this->layers.size(); i++)
            {
                this->layers[i]->InitializeGL(this, f);
                GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->InitializeGL");
                this->layers[i]->SetTextRenderer(textRenderer);
            }
            f->glActiveTexture(GL_TEXTURE0);


        }

        void Viewer2D::resizeGL(int w, int h)
        {
            if (!this->operationRunner.expired())
            {
                this->operationRunner.lock()->RunOperation(std::make_shared<ChangeAspectMatrix>(w, h));
            }

            std::unique_ptr<QOpenGLFunctions, void (*)(void *)> f(QOpenGLContext::currentContext()->functions(),
                                                                  DeleteNothing);
            f->glViewport(0, 0, w, h);
        }

        QSize Viewer2D::minimumSizeHint() const
        {
            return QSize(50, 50);
        }

        QSize Viewer2D::sizeHint() const
        {
            return QSize(400, 400);
        }

        void Viewer2D::UpdateFromModel(std::shared_ptr<Model> const &model)
        {
            std::unique_ptr<QOpenGLFunctions, void (*)(void *)> f(QOpenGLContext::currentContext()->functions(),
                                                                  DeleteNothing);

            if (model->settings->IsChanged())
            {
                QColor clearColor = model->colorScheme->EditorBackgroundColor();
                f->glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
            }

            this->loaded = model->documentAccess->IsDocumentLoaded();
            if(this->loaded)
            {
                for (int i = 0; i < this->layers.size(); i++)
                {
                    this->layers[i]->UpdateScene(model, f);
                    GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->UpdateScene");
                }
            }
            f->glActiveTexture(GL_TEXTURE0);
        }

        MinMaxValue::MinMaxValue()
        {

        }

        MinMaxValue::MinMaxValue(QVector2D min, QVector2D max)
        {
            this->min = min;
            this->max = max;
            this->Active = true;
        }

        MinMaxValue MinMaxValue::Combine(MinMaxValue first, MinMaxValue second)
        {
            using namespace boost::numeric::ublas;
            MinMaxValue result;
            if (!first.Active && !second.Active)
            {
                result.Active = false;
            }
            else if (!first.Active)
            {
                for (int i = 0; i < 2; i++)
                {
                    result.min[i] = second.min[i];
                    result.max[i] = second.max[i];
                }
            }
            else if (!second.Active)
            {
                for (int i = 0; i < 2; i++)
                {
                    result.min[i] = first.min[i];
                    result.max[i] = first.max[i];
                }
            }
            else
            {
                for (int i = 0; i < 2; i++)
                {
                    result.min[i] = std::min(first.min[i], second.min[i]);
                    result.max[i] = std::max(first.max[i], second.max[i]);
                }
            }
            return result;
        }

        MinMaxValue MinMaxValue::CombineList(std::vector<MinMaxValue> list)
        {
            MinMaxValue result = list.back();
            list.pop_back();
            for (MinMaxValue v: list)
            {
                result = Combine(result, v);
            }
            return result;
        }

        void Viewer2D::mousePressEvent(QMouseEvent *event)
        {
            QPoint ScreenPos = event->pos();
            QVector2D pos = QVector2D(ScreenPos.x() / ((float) this->width()) * 2 - 1,
                                      -1 * (ScreenPos.y() / ((float) this->height()) * 2 - 1));

            std::shared_ptr<LambdaOperation> op = std::make_shared<LambdaOperation>([&](const Reciever &reciever) {
                reciever.model->mouseHandler->mousePressEvent(reciever.model, event, pos);
            });
            this->operationRunner.lock()->RunOperation(op);
        }

        void Viewer2D::mouseReleaseEvent(QMouseEvent *event)
        {
            QPoint ScreenPos = event->pos();
            QVector2D pos = QVector2D(ScreenPos.x() / ((float) this->width()) * 2 - 1,
                                      -1 * (ScreenPos.y() / ((float) this->height()) * 2 - 1));

            std::shared_ptr<LambdaOperation> op = std::make_shared<LambdaOperation>([&](const Reciever &reciever) {
                reciever.model->mouseHandler->mouseReleaseEvent(reciever.model, event, pos);
            });
            this->operationRunner.lock()->RunOperation(op);
        }

        void Viewer2D::mouseMoveEvent(QMouseEvent *event)
        {
            QPoint ScreenPos = event->pos();
            QVector2D pos = QVector2D(ScreenPos.x() / ((float) this->width()) * 2 - 1,
                                      -1 * (ScreenPos.y() / ((float) this->height()) * 2 - 1));

            std::shared_ptr<LambdaOperation> op = std::make_shared<LambdaOperation>([&](const Reciever &reciever) {
                reciever.model->mouseHandler->mouseMoveEvent(reciever.model, event, pos);
            });
            this->operationRunner.lock()->RunOperation(op);
        }

        void Viewer2D::wheelEvent(QWheelEvent *event)
        {
            QPoint ScreenPos = event->pos();
            QVector2D pos = QVector2D(ScreenPos.x() / ((float) this->width()) * 2 - 1,
                                      -1 * (ScreenPos.y() / ((float) this->height()) * 2 - 1));

            std::shared_ptr<LambdaOperation> op = std::make_shared<LambdaOperation>([&](const Reciever &reciever) {
                reciever.model->mouseHandler->wheelEvent(reciever.model, event, pos);
            });
            this->operationRunner.lock()->RunOperation(op);
        }

    }
}

OpenPSTD::GUI::TextRenderer::TextRenderer(std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
{
    std::map<char, QRect> iTextureMapping = this->GetIntegerTextureMapping();

    std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(
            new std::string(":/GPU/Text.vert"));
    std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(
            new std::string(":/GPU/Text.frag"));

    program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
    program->link();

    program->bind();

    f->glUniform1i((GLuint) program->attributeLocation("font"), 0);//set to texture unit 0

    QImage font(":/Images/arial_0.png");

    char AsciiFirstReadingSymbol = 32;
    char AsciiLastReadingSymbol = 126;
    for(char c = AsciiFirstReadingSymbol; c <= AsciiLastReadingSymbol; c++)
    {
        QRect boundingRect = iTextureMapping[c];
        this->textureMapping[c] = QRectF(boundingRect.left()/(float)font.width(),
                                         boundingRect.top()/(float)font.height(),
                                         boundingRect.width()/(float)font.width(),
                                         boundingRect.height()/(float)font.height());
    }

    f->glGenTextures(1, &this->textureID);
    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, this->textureID);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font.width(), font.height(), 0, GL_RED, GL_UNSIGNED_BYTE, font.bits());

    f->glGenBuffers(1, &this->textureCoordsBuffer);
    f->glGenBuffers(1, &this->positionsBuffer);
}

void OpenPSTD::GUI::TextRenderer::Draw(std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f,
                                       QMatrix4x4 viewMatrix, QVector2D position, float height, std::string s,
                                       QColor color, TextVerticalAlignment vAlign, TextHorizontalAlignment hAlign)
{
    double totalWidth = this->GetWidth(height, s);
    switch(hAlign)
    {
        case TextHorizontalAlignment::LEFT:
            position[0] = position[0] - (float)totalWidth;
            break;
        case TextHorizontalAlignment::CENTER:
            position[0] = position[0] - (float)totalWidth/2;
            break;
        case TextHorizontalAlignment::RIGHT:
            break;
    }

    switch(vAlign)
    {
        case TextVerticalAlignment::TOP:
            position[1] = position[1] - (float)height;
            break;
        case TextVerticalAlignment::CENTER:
            position[1] = position[1] - (float)height/2;
            break;
        case TextVerticalAlignment::BOTTOM:
            break;
    }

    std::vector<QVector2D> textureCoords;
    std::vector<QVector2D> positions;
    double nextCharX = 0;

    textureCoords.reserve(s.length() * 6);
    positions.reserve(s.length() * 6);

    for (unsigned int i = 0; i < s.length(); i++)
    {
        char c = s[i];
        QRectF rect = this->textureMapping[c];

        double sizeFactor = height / rect.height();

        double left = position.x() + nextCharX;
        double right = left + rect.width() * sizeFactor;
        double bottom = position.y();
        double top = position.y() + height;

        nextCharX += rect.width() * sizeFactor;

        QVector2D tl = QVector2D((float) left, (float) top);
        QVector2D tr = QVector2D((float) right, (float) top);
        QVector2D bl = QVector2D((float) left, (float) bottom);
        QVector2D br = QVector2D((float) right, (float) bottom);

        positions.push_back(tl);
        positions.push_back(tr);
        positions.push_back(bl);

        positions.push_back(tr);
        positions.push_back(br);
        positions.push_back(bl);

        //top and bottom is replaced, this is due the fact that the screen is upside down, because of the file format
        textureCoords.push_back(QVector2D(rect.bottomLeft()));
        textureCoords.push_back(QVector2D(rect.bottomRight()));
        textureCoords.push_back(QVector2D(rect.topLeft()));

        textureCoords.push_back(QVector2D(rect.bottomRight()));
        textureCoords.push_back(QVector2D(rect.topRight()));
        textureCoords.push_back(QVector2D(rect.topLeft()));
    }
    program->bind();
    program->enableAttributeArray("texcoord");
    program->enableAttributeArray("position");

    program->setUniformValue("view", viewMatrix);
    program->setUniformValue("color", color);

    f->glBindBuffer(GL_ARRAY_BUFFER, this->textureCoordsBuffer);
    GLError("TextRenderer f->glBindBuffer");
    f->glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(QVector2D), textureCoords.data(), GL_STREAM_DRAW);
    GLError("TextRenderer f->glBufferData");
    f->glVertexAttribPointer((GLuint) program->attributeLocation("texcoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
    GLError("TextRenderer f->glVertexAttribPointer");

    f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
    GLError("TextRenderer f->glBindBuffer");
    f->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(QVector2D), positions.data(), GL_STREAM_DRAW);
    GLError("TextRenderer f->glBufferData");
    f->glVertexAttribPointer((GLuint) program->attributeLocation("position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
    GLError("TextRenderer f->glVertexAttribPointer");

    f->glActiveTexture(GL_TEXTURE0);
    GLError("TextRenderer f->glActiveTexture");

    f->glBindTexture(GL_TEXTURE_2D, this->textureID);
    GLError("TextRenderer f->glBindTexture");

    f->glDrawArrays(GL_TRIANGLES, 0, positions.size());
    GLError("TextRenderer f->glDrawArrays");

    program->disableAttributeArray("texcoord");
    program->disableAttributeArray("position");
}

std::map<char, QRect> OpenPSTD::GUI::TextRenderer::GetIntegerTextureMapping()
{
    std::map<char, QRect> iTextureMapping;

    iTextureMapping[	32	]=QRect(	106	,	165	,	8	,	32	);
    iTextureMapping[	33	]=QRect(	151	,	165	,	8	,	32	);
    iTextureMapping[	34	]=QRect(	206	,	132	,	10	,	32	);
    iTextureMapping[	35	]=QRect(	106	,	66	,	16	,	32	);
    iTextureMapping[	36	]=QRect(	191	,	66	,	15	,	32	);
    iTextureMapping[	37	]=QRect(	57	,	0	,	24	,	32	);
    iTextureMapping[	38	]=QRect(	180	,	33	,	18	,	32	);
    iTextureMapping[	39	]=QRect(	250	,	132	,	5	,	32	);
    iTextureMapping[	40	]=QRect(	0	,	165	,	9	,	32	);
    iTextureMapping[	41	]=QRect(	30	,	165	,	9	,	32	);
    iTextureMapping[	42	]=QRect(	194	,	132	,	11	,	32	);
    iTextureMapping[	43	]=QRect(	174	,	66	,	16	,	32	);
    iTextureMapping[	44	]=QRect(	115	,	165	,	8	,	32	);
    iTextureMapping[	45	]=QRect(	20	,	165	,	9	,	32	);
    iTextureMapping[	46	]=QRect(	124	,	165	,	8	,	32	);
    iTextureMapping[	47	]=QRect(	217	,	132	,	10	,	32	);
    iTextureMapping[	48	]=QRect(	207	,	66	,	15	,	32	);
    iTextureMapping[	49	]=QRect(	223	,	66	,	15	,	32	);
    iTextureMapping[	50	]=QRect(	239	,	66	,	15	,	32	);
    iTextureMapping[	51	]=QRect(	0	,	99	,	15	,	32	);
    iTextureMapping[	52	]=QRect(	16	,	99	,	15	,	32	);
    iTextureMapping[	53	]=QRect(	32	,	99	,	15	,	32	);
    iTextureMapping[	54	]=QRect(	128	,	99	,	15	,	32	);
    iTextureMapping[	55	]=QRect(	48	,	99	,	15	,	32	);
    iTextureMapping[	56	]=QRect(	64	,	99	,	15	,	32	);
    iTextureMapping[	57	]=QRect(	80	,	99	,	15	,	32	);
    iTextureMapping[	58	]=QRect(	97	,	165	,	8	,	32	);
    iTextureMapping[	59	]=QRect(	88	,	165	,	8	,	32	);
    iTextureMapping[	60	]=QRect(	123	,	66	,	16	,	32	);
    iTextureMapping[	61	]=QRect(	140	,	66	,	16	,	32	);
    iTextureMapping[	62	]=QRect(	157	,	66	,	16	,	32	);
    iTextureMapping[	63	]=QRect(	96	,	99	,	15	,	32	);
    iTextureMapping[	64	]=QRect(	29	,	0	,	27	,	32	);
    iTextureMapping[	65	]=QRect(	122	,	33	,	19	,	32	);
    iTextureMapping[	66	]=QRect(	237	,	0	,	18	,	32	);
    iTextureMapping[	67	]=QRect(	0	,	33	,	20	,	32	);
    iTextureMapping[	68	]=QRect(	21	,	33	,	20	,	32	);
    iTextureMapping[	69	]=QRect(	161	,	33	,	18	,	32	);
    iTextureMapping[	70	]=QRect(	0	,	66	,	17	,	32	);
    iTextureMapping[	71	]=QRect(	129	,	0	,	21	,	32	);
    iTextureMapping[	72	]=QRect(	102	,	33	,	19	,	32	);
    iTextureMapping[	73	]=QRect(	79	,	165	,	8	,	32	);
    iTextureMapping[	74	]=QRect(	140	,	132	,	13	,	32	);
    iTextureMapping[	75	]=QRect(	82	,	33	,	19	,	32	);
    iTextureMapping[	76	]=QRect(	112	,	99	,	15	,	32	);
    iTextureMapping[	77	]=QRect(	82	,	0	,	23	,	32	);
    iTextureMapping[	78	]=QRect(	62	,	33	,	19	,	32	);
    iTextureMapping[	79	]=QRect(	173	,	0	,	21	,	32	);
    iTextureMapping[	80	]=QRect(	218	,	33	,	17	,	32	);
    iTextureMapping[	81	]=QRect(	151	,	0	,	21	,	32	);
    iTextureMapping[	82	]=QRect(	216	,	0	,	20	,	32	);
    iTextureMapping[	83	]=QRect(	142	,	33	,	18	,	32	);
    iTextureMapping[	84	]=QRect(	72	,	66	,	16	,	32	);
    iTextureMapping[	85	]=QRect(	42	,	33	,	19	,	32	);
    iTextureMapping[	86	]=QRect(	18	,	66	,	17	,	32	);
    iTextureMapping[	87	]=QRect(	0	,	0	,	28	,	32	);
    iTextureMapping[	88	]=QRect(	36	,	66	,	17	,	32	);
    iTextureMapping[	89	]=QRect(	199	,	33	,	18	,	32	);
    iTextureMapping[	90	]=QRect(	54	,	66	,	17	,	32	);
    iTextureMapping[	91	]=QRect(	142	,	165	,	8	,	32	);
    iTextureMapping[	92	]=QRect(	60	,	165	,	9	,	32	);
    iTextureMapping[	93	]=QRect(	70	,	165	,	8	,	32	);
    iTextureMapping[	94	]=QRect(	181	,	132	,	12	,	32	);
    iTextureMapping[	95	]=QRect(	236	,	33	,	17	,	32	);
    iTextureMapping[	96	]=QRect(	10	,	165	,	9	,	32	);
    iTextureMapping[	97	]=QRect(	144	,	99	,	15	,	32	);
    iTextureMapping[	98	]=QRect(	160	,	99	,	15	,	32	);
    iTextureMapping[	99	]=QRect(	110	,	132	,	14	,	32	);
    iTextureMapping[	100	]=QRect(	176	,	99	,	15	,	32	);
    iTextureMapping[	101	]=QRect(	192	,	99	,	15	,	32	);
    iTextureMapping[	102	]=QRect(	228	,	132	,	10	,	32	);
    iTextureMapping[	103	]=QRect(	208	,	99	,	15	,	32	);
    iTextureMapping[	104	]=QRect(	32	,	132	,	15	,	32	);
    iTextureMapping[	105	]=QRect(	175	,	165	,	6	,	32	);
    iTextureMapping[	106	]=QRect(	160	,	165	,	7	,	32	);
    iTextureMapping[	107	]=QRect(	95	,	132	,	14	,	32	);
    iTextureMapping[	108	]=QRect(	168	,	165	,	6	,	32	);
    iTextureMapping[	109	]=QRect(	106	,	0	,	22	,	32	);
    iTextureMapping[	110	]=QRect(	224	,	99	,	15	,	32	);
    iTextureMapping[	111	]=QRect(	240	,	99	,	15	,	32	);
    iTextureMapping[	112	]=QRect(	0	,	132	,	15	,	32	);
    iTextureMapping[	113	]=QRect(	16	,	132	,	15	,	32	);
    iTextureMapping[	114	]=QRect(	239	,	132	,	10	,	32	);
    iTextureMapping[	115	]=QRect(	80	,	132	,	14	,	32	);
    iTextureMapping[	116	]=QRect(	133	,	165	,	8	,	32	);
    iTextureMapping[	117	]=QRect(	48	,	132	,	15	,	32	);
    iTextureMapping[	118	]=QRect(	64	,	132	,	15	,	32	);
    iTextureMapping[	119	]=QRect(	195	,	0	,	20	,	32	);
    iTextureMapping[	120	]=QRect(	168	,	132	,	12	,	32	);
    iTextureMapping[	121	]=QRect(	125	,	132	,	14	,	32	);
    iTextureMapping[	122	]=QRect(	154	,	132	,	13	,	32	);
    iTextureMapping[	123	]=QRect(	40	,	165	,	9	,	32	);
    iTextureMapping[	124	]=QRect(	182	,	165	,	6	,	32	);
    iTextureMapping[	125	]=QRect(	50	,	165	,	9	,	32	);
    iTextureMapping[	126	]=QRect(	89	,	66	,	16	,	32	);

    return iTextureMapping;
}

double OpenPSTD::GUI::TextRenderer::GetWidth(float height, std::string s)
{
    double width = 0;
    for (unsigned int i = 0; i < s.length(); i++)
    {
        char c = s[i];
        QRectF rect = this->textureMapping[c];

        double sizeFactor = height / rect.height();
        double charWidth = rect.width() * sizeFactor;

        width += charWidth;
    }
    return width;
}







