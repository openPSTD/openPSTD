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
// Date:
//      18-7-2015
//
// Authors:
//      michiel
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_VIEWER2D_H
#define OPENPSTD_VIEWER2D_H

#include <QOpenGLWidget>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <algorithm>
#include "Model.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "operations/BaseOperation.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

namespace OpenPSTD
{
    namespace GUI
    {
        class Layer;

        enum class TextVerticalAlignment
        {
            TOP,
            CENTER,
            BOTTOM
        };
        enum class TextHorizontalAlignment
        {
            LEFT,
            CENTER,
            RIGHT
        };

        class TextRenderer
        {
        private:
            GLuint textureID;
            GLuint textureCoordsBuffer;
            GLuint positionsBuffer;
            std::map<char, QRectF> textureMapping;
            std::unique_ptr<QOpenGLShaderProgram> program;

            std::map<char, QRect> GetIntegerTextureMapping();

        public:
            TextRenderer(std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f);

            void Draw(std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f, QMatrix4x4 viewMatrix,
                      QVector2D position, float height, std::string string, QColor color = QColor(0, 0, 0, 255),
                      TextVerticalAlignment vAlign = TextVerticalAlignment::CENTER,
                      TextHorizontalAlignment hAlign = TextHorizontalAlignment::CENTER);

            /**
             * Gets the size in world space
             */
            double GetWidth(float height, std::string string);
        };

        class MinMaxValue
        {
        public:
            MinMaxValue();

            MinMaxValue(QVector2D min, QVector2D max);

            QVector2D min;
            QVector2D max;
            bool Active;

            static MinMaxValue Combine(MinMaxValue first, MinMaxValue second);

            static MinMaxValue CombineList(std::vector<MinMaxValue> list);
        };

        void DeleteNothing(void *ptr);

        void DeleteTexture(void *ptr);

        class Viewer2D : public QOpenGLWidget
        {
        Q_OBJECT

        public:
            explicit Viewer2D(QWidget *parent = 0);

            virtual QSize sizeHint() const override;

            virtual QSize minimumSizeHint() const override;

            void UpdateFromModel(std::shared_ptr<Model> const &model);

            void SetOperationRunner(std::weak_ptr<OperationRunner> operationRunner);


        protected:
            virtual void resizeGL(int w, int h) override;

            virtual void initializeGL() override;

            virtual void paintGL() override;

            virtual void mousePressEvent(QMouseEvent *mouseEvent) override;

            virtual void mouseReleaseEvent(QMouseEvent *mouseEvent) override;

            virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;

            virtual void wheelEvent(QWheelEvent *qWheelEvent) override;

        private:

            QOpenGLVertexArrayObject * _vao;
            std::shared_ptr<TextRenderer> textRenderer;
            std::vector<std::shared_ptr<Layer>> layers;
            std::weak_ptr<OperationRunner> operationRunner;
            bool loaded;
        };

        void GLError(std::string name);

        class Layer
        {
        protected:
            bool visible;
            std::shared_ptr<TextRenderer> textRenderer;

        public:
            Layer(): visible(true) {}

            virtual bool GetVisible()
            { return visible; };

            virtual void SetVisible(bool value)
            { visible = value; };

            virtual void SetTextRenderer(std::shared_ptr<TextRenderer> value)
            { textRenderer = value; };

            virtual void InitializeGL(QObject *context,
                                      std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f) = 0;

            virtual void PaintGLVisibilityCheck(QObject *context,
                                                std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
            {
                if (visible)
                { this->PaintGL(context, f); }
            };

            virtual void PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f) = 0;

            virtual void UpdateScene(std::shared_ptr<Model> const &m,
                                     std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f) = 0;

            virtual MinMaxValue GetMinMax() = 0;
        };



    }
}

class ui_Viewer2D : public OpenPSTD::GUI::Viewer2D {};


#endif //OPENPSTD_VIEWER2D_H
