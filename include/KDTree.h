//
// Created by Ryan Strauss on 6/30/20.
// Modified by V. Prins 2021-07-16
//

#ifndef KDTree_H
#define KDTree_H

#include <vector>
#include <memory>
#include "Particle.h"

class Node
{
public:
    using NodePtr = std::shared_ptr<Node>;

    NodePtr left, right;
    Particle *particle;
    bool vertical;

    Node(Particle *particle, bool vertical, const NodePtr &left, const NodePtr &right);

    ~Node();

    bool isLeaf() const;
};

class KDTree
{
private:
    float width, height;

    Node::NodePtr root;

    Node::NodePtr insert(const Node::NodePtr &node, Particle *particle, bool vertical);

    void search(Particle *query, double radius, const Node::NodePtr &node, std::vector<Particle *> &results) const;

public:
    KDTree(float width, float height);

    void insert(Particle *particle);

    std::vector<Particle *> search(Particle *query, double radius) const;
};

#endif
