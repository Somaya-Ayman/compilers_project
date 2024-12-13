#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include <QString>
#include <QList>
#include <QGraphicsScene>
#include <set>

// Represents a single node in the syntax tree
class SyntaxTreeNode {
public:
    QString name;
    QList<SyntaxTreeNode*> children = {};
    SyntaxTreeNode* sibling = NULL;         // Pointer to the next sibling
    bool processed = false;

    SyntaxTreeNode(const QString &name);
    ~SyntaxTreeNode();

    // Recursively add the tree to a QGraphicsScene for visualization
    void addToScene(QGraphicsScene *scene, int &x, int y, int xSpacing, int ySpacing);

    void deleteNode(SyntaxTreeNode* node, std::set<SyntaxTreeNode*>& visited);
    int getDepth() const;
    int getMaxWidth() const;
};

#endif // SYNTAXTREE_H
