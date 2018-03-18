#ifndef DOMAIN_H
#define DOMAIN_H

#include <QImage>
#include <QPen>
#include <QPainter>
#include <vector>
#include "wall.h"
#include "settings.h"

/**
 * Representation of a single domain.
 */
class Domain {
public:
    // Constructor
    Domain(int x0, int y0, int x1, int y1);
    
    // Drawing method
    void draw(QImage* pixels, int zoom, int offsetX, int offsetY);
    
    // Get methods for the domain's corner coordinates
    inline int getX0() { return std::min(x0, x1); }
    inline int getY0() { return std::min(y0, y1); }
    inline int getX1() { return std::max(x0, x1); }
    inline int getY1() { return std::max(y0, y1); }
    inline int getTrueX0() { return x0; }
    inline int getTrueX1() { return x1; }
    inline int getTrueY0() { return y0; }
    inline int getTrueY1() { return y1; }
    
    // Set methods for the domain's corner coordinates
    inline void setX0(int x0) { this->x0 = x0; }
    inline void setY0(int y0) { this->y0 = y0; }
    inline void setX1(int x1) { this->x1 = x1; }
    inline void setY1(int y1) { this->y1 = y1; }
    
    // Get method for the walls vector
    std::vector<Wall*>* getWalls() { return &walls; }
    
    // Merges this domain with all other domains
    void mergeDomains(std::vector<Domain>* domains, unsigned int ownID);
    
    // Resets the domain's walls to be the original four non-merged walls
    void resetWalls();
    
    // Checks whether or not this domain overlaps with another domain
    bool overlaps(Domain* other);
    
    // Get method for the absorption of a wall given its side
    double getAbsorption(Side side);
    void updateAbsorption();
    void setAbsorption(Side side, double value);
    
    bool getSelectedDomain() { return selected_domain; }
    bool getSelectedTop() { return selected_top; }
    bool getSelectedBottom() { return selected_bottom; }
    bool getSelectedLeft() { return selected_left; }
    bool getSelectedRight() { return selected_right; }
    void setSelectedDomain(bool selected) { selected_domain = selected; }
    void setSelectedTop(bool selected) { selected_top = selected; }
    void setSelectedBottom(bool selected) { selected_bottom = selected; }
    void setSelectedLeft(bool selected) { selected_left = selected; }
    void setSelectedRight(bool selected) { selected_right = selected; }
private:
    // Domain walls
    int x0;
    int y0;
    int x1;
    int y1;
    std::vector<Wall*> walls;
    
    // Absorption coefficients
    double absorptionTop;
    double absorptionBottom;
    double absorptionLeft;
    double absorptionRight;
    
    // State variables
    bool selected_domain;
    bool selected_top;
    bool selected_bottom;
    bool selected_left;
    bool selected_right;
    
    // Method for updating the side of all walls
    void updateWallSides();
    
    // Updates a given wall according to a given intersecting segment
    void handleIntersection(Domain* parent, int wallID, std::pair<int, int> toMerge);
};

#endif