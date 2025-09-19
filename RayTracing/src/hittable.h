#pragma once
#include "Utilities.h"

#include "aabb.h"
class material;
struct hit_record {
    glm::vec3 p;
    glm::vec3 normal;
    float t;
    std::shared_ptr<material> mat_ptr;
    double u;
    double v;

    bool front_face;

    inline void set_face_normal(const ray& r, const glm::vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {

    public: 
        glm::vec3 center;
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
        virtual bool bounding_box( aabb& output_box) const = 0;

};

class translate : public hittable {
public:
    translate(shared_ptr<hittable> p, const glm::vec3& displacement)
        : ptr(p), offset(displacement) {}

    virtual bool hit(
        const ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box( aabb& output_box) const override;

public:
   std::shared_ptr<hittable> ptr;
    glm::vec3 offset;
};

inline bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    ray moved_r(r.origin() - offset, r.direction());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

inline bool translate::bounding_box( aabb& output_box) const {
    if (!ptr->bounding_box( output_box))
        return false;

    output_box = aabb(
        output_box.min() + offset,
        output_box.max() + offset);

    return true;
}