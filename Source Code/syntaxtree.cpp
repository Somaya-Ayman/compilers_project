#include "syntaxtree.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QMap>
#include <QVector>

// Constructor
SyntaxTreeNode::SyntaxTreeNode(const QString &name) : name(name) {}

SyntaxTreeNode::~SyntaxTreeNode() {
    std::set<SyntaxTreeNode*> visited;
    deleteNode(this, visited);
}

void SyntaxTreeNode::deleteNode(SyntaxTreeNode* node, std::set<SyntaxTreeNode*>& visited) {
    if (visited.find(node) != visited.end()) {
        return; // Avoid infinite recursion
    }
    visited.insert(node);

    for (SyntaxTreeNode* child : node->children) {
        deleteNode(child, visited);
    }

    if (node->sibling) {
        deleteNode(node->sibling, visited);
    }

    delete node;
}

// Helper function to determine if a node is a non-terminal
static bool isNonTerminal(const QString &name) {
    // Non-terminals are rule names, typically in PascalCase or with descriptive names.
    // Update this logic if needed to match your grammar.
    return name.contains("assign") ||name.contains("write")
           || name.contains("read") || name.contains("repeat")
           || name == "if" || name == "else";
}


void SyntaxTreeNode::addToScene(QGraphicsScene *scene, int &x, int y, int xSpacing, int ySpacing) {
    int nodeRadius = 20; // For circles
    int rectWidth = 80;  // For rectangles
    int rectHeight = 40;
    static int oldx = x;
    if (processed) {
        return;
    }
    processed = true;

    qDebug() << "Adding node:" << name << "at position (" << x << "," << y << ")";

    // Draw the current node
    if (isNonTerminal(name)) {
        scene->addRect(x - rectWidth / 2, y - rectHeight / 2, rectWidth, rectHeight);
        QGraphicsTextItem* text = scene->addText(name);
        text->setPos(x - text->boundingRect().width() / 2, y - text->boundingRect().height() / 2);
    } else {
        scene->addEllipse(x - nodeRadius, y - nodeRadius, nodeRadius * 2, nodeRadius * 2);
        QGraphicsTextItem* text = scene->addText(name);
        text->setPos(x - text->boundingRect().width() / 2, y - text->boundingRect().height() / 2);
    }

    // Calculate starting x position for children
    int currentX = x - (getMaxWidth() * xSpacing) / 2;

    // Recursively add children
    for (SyntaxTreeNode *child : children) {
        qDebug() << "Adding children";
        // Calculate the width of the subtree rooted at this child
        int subtreeWidth = child->getMaxWidth() * xSpacing;
        // Center the child node under the current node
        int childX = currentX + subtreeWidth / 2;

        // Draw a line connecting this node to the child
        scene->addLine(x, y + (isNonTerminal(name) ? rectHeight / 2 : nodeRadius),
                       childX, y + ySpacing - (isNonTerminal(name) ? rectHeight / 2 : nodeRadius));

        // Recursively add the child node
        qDebug() << "child tot width??" <<subtreeWidth <<"name" <<child->name;

        qDebug() << "Adding child node:" << child->name << "at position (" << childX << "," << y << ")";
        child->addToScene(scene, childX, y + ySpacing, xSpacing, ySpacing);
        if (child->sibling) {
            int siblingWidth = 0;
            for (size_t i = 0; i < child->sibling->children.size(); ++i) {
            siblingWidth += child->sibling->getMaxWidth();
            }
            currentX += subtreeWidth + siblingWidth*100 ; // Start siblings to the right of the current node

        }
        else{
            currentX += subtreeWidth + 100;;
        }

    }
    int h = x;
    x = currentX;


    // Draw siblings
    SyntaxTreeNode* currentSibling = sibling;
    int siblingX = x + 100; // Start position for the first sibling

    while (currentSibling) {
        if (currentSibling->processed) {
            return; // Avoid reprocessing siblings
        }

        // Calculate width of the current sibling's subtree
        int siblingSubtreeWidth = currentSibling->getMaxWidth() * xSpacing;

        // Center siblingX for this sibling
        siblingX += siblingSubtreeWidth / 2;

        qDebug() << "Adding sibling node:" << currentSibling->name << "at position (" << siblingX << "," << y << ")";

        // Draw horizontal line connecting siblings
        int startX = isNonTerminal(name) ? h + rectWidth/2  : x + nodeRadius;
        int endX = isNonTerminal(currentSibling->name) ? siblingX - rectWidth / 2 : siblingX - nodeRadius;
        scene->addLine(startX, y, endX, y);

        // Recursively add sibling
        currentSibling->addToScene(scene, siblingX, y, xSpacing, ySpacing);

        // Update siblingX for the next sibling
        siblingX += siblingSubtreeWidth / 2 + xSpacing;

        // Proceed to the next sibling
        currentSibling = currentSibling->sibling;
    }


}

int SyntaxTreeNode::getDepth() const {
    int maxDepth = 0;
    int spacing = 0;
    for (const SyntaxTreeNode* child : children) {
        maxDepth = std::max(maxDepth, child->getDepth());
    }
    spacing = 200*children.size();
    return maxDepth + 1; // Add 1 for the current node
}


int SyntaxTreeNode::getMaxWidth() const {
    int totalWidth = 0;
    for (const SyntaxTreeNode* child : children) {
        totalWidth += child->getMaxWidth();
    }
    if (sibling) {
        totalWidth += sibling->getMaxWidth();
    }
    return std::max(1, totalWidth); // Ensure at least width of 1 for leaf nodes
}
