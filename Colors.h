//
// Created by michiel on 30-8-2015.
//

#ifndef OPENPSTD_COLORS_H
#define OPENPSTD_COLORS_H

#include <qcolor.h>
#include <vector>
#include <memory>
#include <map>

/**
 * A base class that calculates color gradients
 */
class BaseColorGradient
{
public:
    /**
     * Calculates color from a value in a certain gradient
     * @param value the value that is as basis of the gradient calculation. This value should be between 0 and 1 (including)
     * @return The color
     */
    virtual QColor CalcColor(float value) = 0;

    /**
     * Creates a complete color map,
     * @param start The minimal value of the colormap
     * @param stop The maximal value of the colormap
     * @param count The number of samples taken
     * @return the complete colormap
     */
    virtual std::unique_ptr<std::vector<QColor>> CreateColorMap(float start, float stop, unsigned int count);

    /**
     * Creates a raw color map with RGBA values.
     * @param start The minimal value of the colormap
     * @param stop The maximal value of the colormap
     * @param count The number of samples taken
     * @return the complete raw colormap
     */
    std::unique_ptr<std::vector<float>> CreateRawRGBAColorMap(float start, float stop, unsigned int count);
};

/**
 * A multicolor gradient where there are multiple colors at certain locations.
 * Outside the 2 outer colors, the color is constant based on the closest of color.
 */
class MultiColorGradient : public BaseColorGradient
{
public:
    /**
     * Adds a color the the gradient at a certain position
     * @param position The position of the color, this should be between 0 and 1 (including)
     * @param c The color at that position
     */
    void AddColor(float position, QColor c);

    virtual QColor CalcColor(float value);

private:
    std::map<float, QColor> colors;
    float LinearInterpolation(float x0, float y0, float x1, float y1, float x);
};

/**
 * A 2-color gradient where 1 color is used at 0 and the other color is used at 1.
 * This class is a specilized case of the MultiColorGradient
 */
class TwoColorGradient : public MultiColorGradient
{
public:
    TwoColorGradient(QColor c0, QColor c1);
};

#define COLOR_BLACK QColor(0, 0, 0, 255)
#define COLOR_WHITE QColor(255, 255, 255, 255)
#define COLOR_RED QColor(255, 0, 0, 255)
#define COLOR_GREEN QColor(0, 255, 0, 255)
#define COLOR_BLUE QColor(0, 0, 255, 255)
#define COLOR_YELLOW QColor(255, 255, 0, 255)
#define COLOR_LIGHT_BLUE QColor(0, 255, 255, 255)
#define COLOR_PURPLE QColor(255, 0, 255, 255)
#define COLOR_TRANSPARENT QColor(0, 0, 0, 0)
#define COLOR_GRADIENT_WHITE std::unique_ptr<TwoColorGradient>(new TwoColorGradient(COLOR_WHITE, COLOR_WHITE))

/**
 * A base color scheme, just defines the property's
 */
class BaseColorScheme
{
public:
    virtual QColor EditorBackgroundColor();
    virtual QColor EditorLineColor();
    virtual std::unique_ptr<BaseColorGradient> EditorLineAbsoptionColorGradient();
    virtual QColor EditorDefualtDomainColor();
    virtual std::unique_ptr<BaseColorGradient> EditorDomainSignalColorGradient();
    virtual QColor EditorAddDomainColor();
    virtual QColor EditorSelectionColor();
    virtual QColor EditorSpeakerColor();
    virtual QColor EditorReceiverColor();

};

/**
 * The default color scheme
 */
class StandardColorScheme: public BaseColorScheme
{
public:
    virtual QColor EditorBackgroundColor();

    virtual QColor EditorLineColor();

    virtual std::unique_ptr<BaseColorGradient> EditorLineAbsoptionColorGradient();

    virtual QColor EditorDefualtDomainColor();

    virtual std::unique_ptr<BaseColorGradient> EditorDomainSignalColorGradient();

    virtual QColor EditorAddDomainColor();

    virtual QColor EditorSelectionColor();

    virtual QColor EditorSpeakerColor();

    virtual QColor EditorReceiverColor();

};

#endif //OPENPSTD_COLORS_H
