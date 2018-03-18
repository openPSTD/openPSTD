#include "wall.h"

/**
 * Constructor.
 * 
 * @param x0  The x coordinate of the first end of the wall
 * @param y0  The y coordinate of the first end of the wall
 * @param x1  The x coordinate of the second end of the wall
 * @param y1  The y coordinate of the second end of the wall
 * @param side  The side of the domain that this wall is on
 * @param settings  A reference to a Settings instance
 * @param absorption  The initial absorption coefficient of the wall
 */
Wall::Wall(int x0, int y0, int x1, int y1, Side side, double absorption) {
    // Save end point coordinates locally
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
    this->side = side;
    this->absorption = absorption;
    
    // Set initial absorption coefficient
    absorption = 0;
    
    // Set initial wall length text state
    drawWallLength = true;
}

/**
 * Draws the length of a wall given its end points.
 * 
 * @param pixels  The pixels array to draw to
 * @param zoom  The current zoom level (as in model)
 * @param offsetX  The current x offset of the scene (as in model)
 * @param offsetY  The current y offset of the scene (as in model)
 * @param selected  Whether or not the wall is currently selected
 */
void Wall::draw(QImage* pixels, int zoom, int offsetX, int offsetY, bool selected) {
    // Verify that the wall is orthogonal
    if (x0 != x1 && y0 != y1) {
        std::cerr << "Cannot draw non-orthogonal wall length text" << std::endl;
        return;
    }
    
    // Get the minimum and maximum x and y coordinates of the corners
    int minx = std::min(x0, x1) * zoom + offsetX;
    int maxx = std::max(x0, x1) * zoom + offsetX;
    int miny = -std::max(y0, y1) * zoom + offsetY;
    int maxy = -std::min(y0, y1) * zoom + offsetY;
    
    // Draw all points on the wall
    Settings* settings = Settings::getInstance();
    if (minx == maxx) {
        for (int j = miny; j <= maxy; j++) {
            for (int d = -1; d <= 1; d++) {
                int x = minx + d;
                int y = j;
                QRgb color = gradient(settings->wallColor0, settings->wallColor1, absorption);
                if (selected) color = qRgb(0, 255, 255);
                if (x < 0 || y < 0) continue;
                if (x >= pixels->width() || y >= pixels->height()) continue;
                pixels->setPixel(x, y, color);
            }
        }
    }
    if (miny == maxy) {
        for (int i = minx; i <= maxx; i++) {
            for (int d = -1; d <= 1; d++) {
                int x = i;
                int y = miny + d;
                QRgb color = gradient(settings->wallColor0, settings->wallColor1, absorption);
                if (selected) color = qRgb(0, 255, 255);
                if (x < 0 || y < 0) continue;
                if (x >= pixels->width() || y >= pixels->height()) continue;
                pixels->setPixel(x, y, color);
            }
        }
    }
    
    // Compute the length of the wall
    int length = std::abs((x1 - x0) + (y1 - y0));
    std::string lengthtext = std::to_string(length);
    
    // Compute the midpoint of the wall
    int midx = (minx + maxx) / 2;
    int midy = (miny + maxy) / 2;
    
    // Update midpoint based on the wall's side
    if (side == LEFT) {
        midx -= 12 * lengthtext.size();
    }
    if (side == RIGHT) {
        midx++;
    }
    if (side == TOP) {
        midx -= 5 * lengthtext.size();
        midy -= 9;
    }
    if (side == BOTTOM) {
        midx -= 5 * lengthtext.size();
        midy += 10;
    }
    
    // Draw the wall length text
    if (drawWallLength) {
        drawText(
            lengthtext,
            midx,
            midy - 7,
            14,
            qRgb(0, 0, 0),
            pixels
        );
    }
}

/**
 * Checks whether or not two given walls need to be merged.
 * The algorithm implemented in this method is documented
 * in docs/Wall intersection.docx.
 * 
 * @param one  The first wall to check with
 * @param two  The second wall to check with
 * @param toMerge  A pointer to a location to write the segment that needs to be merged
 * @return  Whether or not the two walls need to be merged
 */
bool Wall::mergeWalls(Wall one, Wall two, std::pair<int, int>* toMerge) {
    // Get the end point coordinates of both walls
    int p1x = one.getX0();
    int p1y = one.getY0();
    int p2x = one.getX1();
    int p2y = one.getY1();
    int p3x = two.getX0();
    int p3y = two.getY0();
    int p4x = two.getX1();
    int p4y = two.getY1();
    
    // Check if the walls are horizontal or vertical
    if (one.getSide() == LEFT || one.getSide() == RIGHT) {
        // Do nothing if the walls are not parallel
        if (two.getSide() != LEFT && two.getSide() != RIGHT) return false;
        
        // Do nothing if the walls do not have the same x coordinate
        if (p1x != p3x) return false;
        
        // Sort points on coordinate
        std::vector<int> original = {p1y, p2y, p3y, p4y};
        std::vector<int> order = sort(original);
        
        // Check if the second segment is on both walls
        bool o1w1 = pointOnWall(original[order[1]], p1y, p2y);
        bool o2w1 = pointOnWall(original[order[2]], p1y, p2y);
        bool o1w2 = pointOnWall(original[order[1]], p3y, p4y);
        bool o2w2 = pointOnWall(original[order[2]], p3y, p4y);
        bool s1w1 = o1w1 && o2w1;
        bool s1w2 = o1w2 && o2w2;
        if (s1w1 && s1w2 && original[order[1]] != original[order[2]]) {
            // The walls intersect between o1 and o2
            toMerge->first = std::min(original[order[1]], original[order[2]]);
            toMerge->second = std::max(original[order[1]], original[order[2]]);
            return true;
        }
        
        // There is no intersection
        return false;
    } else {
        // Do nothing if the walls are not parallel
        if (two.getSide() != TOP && two.getSide() != BOTTOM) return false;
        
        // Do nothing if the walls do not have the same y coordinate
        if (p1y != p3y) return false;
        
        // Sort points on coordinate
        std::vector<int> original = {p1x, p2x, p3x, p4x};
        std::vector<int> order = sort(original);
        
        // Check if the second segment is on both walls
        bool o1w1 = pointOnWall(original[order[1]], p1x, p2x);
        bool o2w1 = pointOnWall(original[order[2]], p1x, p2x);
        bool o1w2 = pointOnWall(original[order[1]], p3x, p4x);
        bool o2w2 = pointOnWall(original[order[2]], p3x, p4x);
        bool s1w1 = o1w1 && o2w1;
        bool s1w2 = o1w2 && o2w2;
        if (s1w1 && s1w2 && original[order[1]] != original[order[2]]) {
            // The walls intersect between o1 and o2
            toMerge->first = std::min(original[order[1]], original[order[2]]);
            toMerge->second = std::max(original[order[1]], original[order[2]]);
            return true;
        }
        
        // There is no intersection
        return false;
    }
}

/**
 * Draws a string to the given pixels array.
 * 
 * @param text  The text to draw
 * @param x  The x position to draw at
 * @param y  The y position to draw at
 * @param size  The font size to draw with
 * @param color  The color to draw in
 * @param pixels  The pixels array to draw to
 */
void Wall::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.setPen(QPen(color));
    p.setFont(QFont("Monospace", size));
    p.drawText(x, y + size, QString(text.c_str()));
    p.end();
}

/**
 * Sorts a vector of integers, and returns a vector containing the indices
 * of the sorted elements in the original vector.
 * 
 * @param original  The vector of integers to sort
 * @return  A vector of indices in the original vector
 */
std::vector<int> Wall::sort(std::vector<int> original) {
    // Create the result vector
    std::vector<int> result;
    
    // Loop while there are unsorted elements
    while (result.size() < original.size()) {
        // Get the minimum element in original that is not yet in result
        int minIndex = -1;
        int minValue = 0;
        for (unsigned int i = 0; i < original.size(); i++) {
            if (std::find(result.begin(), result.end(), i) != result.end()) continue;
            if (minIndex == -1 || original[i] < minValue) {
                minIndex = i;
                minValue = original[i];
            }
        }
        
        // Add the index of this element to result
        result.push_back(minIndex);
    }
    
    // Return the result vector
    return result;
}

/**
 * Returns a gradient between the two given colors, according to
 * a double t in [0, 1].
 * 
 * @param color1  The first color
 * @param color2  The second color
 * @param t  The percentage that the first color should be included
 * @return  A gradient between the two given colors
 */
QRgb Wall::gradient(QRgb color1, QRgb color2, double t) {
    int r1 = QColor(color1).red();
    int r2 = QColor(color2).red();
    int g1 = QColor(color1).green();
    int g2 = QColor(color2).green();
    int b1 = QColor(color1).blue();
    int b2 = QColor(color2).blue();
    
    return qRgb(
        (1-t) * r1 + t * r2,
        (1-t) * g1 + t * g2,
        (1-t) * b1 + t * b2
    );
}