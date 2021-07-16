//
// Created by Ryan Strauss on 6/30/20.
// Modified by V. Prins 2021-07-16
//

#include <cmath>
#include <memory>
#include "KDTree.h"

Node::Node(Particle *particle, bool vertical, const Node::NodePtr &left, const Node::NodePtr &right)
{
    this->particle = particle;
    this->vertical = vertical;
    this->left = left;
    this->right = right;
}

Node::~Node() = default;

bool Node::isLeaf() const
{
    return this->left == nullptr && this->right == nullptr;
}

KDTree::KDTree(float width, float height)
{
    this->root = nullptr;
    this->width = width;
    this->height = height;
}

Node::NodePtr KDTree::insert(const Node::NodePtr &node, Particle *particle, bool vertical)
{
    if (node == nullptr)
    {
        return std::make_shared<Node>(particle, vertical, nullptr, nullptr);
    }

    if ((node->vertical && particle->position.x < node->particle->position.x) ||
        (!node->vertical && particle->position.y < node->particle->position.y))
    {
        node->left = insert(node->left, particle, !node->vertical);
    }
    else
    {
        node->right = insert(node->right, particle, !node->vertical);
    }

    return node;
}

void KDTree::insert(Particle *particle)
{
    root = insert(root, particle, true);
}

double distance2(const Particle *particle, const Node::NodePtr &node)
{
    return std::pow((particle->position.x - node->particle->position.x), 2) +
           std::pow((particle->position.y - node->particle->position.y), 2);
}

void KDTree::search(Particle *query, double radius, const Node::NodePtr &node, std::vector<Particle *> &results) const
{
    double w = query->position.toroidal_distance2(node->particle->position, this->width, this->height);

    if (node->isLeaf())
    {
        if (w < radius * radius)
        {
            results.push_back(node->particle);
        }
        return;
    }

    if (w < radius * radius)
        results.push_back(node->particle);

    double qx = query->position.x;
    double nx = node->particle->position.x;
    double qy = query->position.y;
    double ny = node->particle->position.y;

    if (node->vertical)
    {
        if (qx < nx)
        {
            if (qx - radius <= nx && node->left != nullptr)
            {
                search(query, radius, node->left, results);
            }
            if (qx + radius > nx && node->right != nullptr)
            {
                search(query, radius, node->right, results);
                return;
            }
        }
        else
        {
            if (qx + radius > nx && node->right != nullptr)
            {
                search(query, radius, node->right, results);
            }
            if (qx - radius <= nx && node->left != nullptr)
            {
                search(query, radius, node->left, results);
                return;
            }
        }
    }
    else
    {
        if (qy < ny)
        {
            if (qy - radius <= ny && node->left != nullptr)
            {
                search(query, radius, node->left, results);
            }
            if (qy + radius > ny && node->right != nullptr)
            {
                search(query, radius, node->right, results);
                return;
            }
        }
        else
        {
            if (qy + radius > ny && node->right != nullptr)
            {
                search(query, radius, node->right, results);
            }
            if (qy - radius <= ny && node->left != nullptr)
            {
                search(query, radius, node->left, results);
                return;
            }
        }
    }
}

std::vector<Particle*> KDTree::search(Particle *query, double radius) const
{
    std::vector<Particle*> results;

    bool subtract_width = query->position.x > width/2;
    bool subtract_height = query->position.y > height/2;

    auto p = *query;
    search(&p, radius, root, results);
    p.position.x += (subtract_width) ? -width : width;
    search(&p, radius, root, results);
    p.position.y += (subtract_height) ? -height : height;
    search(&p, radius, root, results);
    p.position.x -= (subtract_width) ? -width : width;
    search(&p, radius, root, results);

    return results;
}
