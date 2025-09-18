#pragma once
#include "hittable.h"
#include "Walnut/Random.h"
class material;
class sphere : public hittable {
public:
    sphere() = default;
    sphere(glm::vec3 cen, float r, shared_ptr<material> m)
        : radius(r), mat_ptr(m) {
        center = cen;
    };
    virtual bool hit(
        const ray& r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box( aabb& output_box) const override;

public:
    float radius;
    shared_ptr<material> mat_ptr;
private:
    static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y);
        auto phi = atan2(-p.z, p.x) + Utils::pi;

        u = phi / (2 * Utils::pi);
        v = theta / Utils::pi;
    }
};
///Calculeaza daca raza r nimereste sfera si daca da returneaza prin rec unde o nimereste
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    
    glm::vec3 oc = r.origin() - center;
    auto half_b = dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    if (half_b * half_b < c) return false;
    auto sqrtd = sqrt(half_b * half_b-c);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) ;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) ;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    glm::vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;
    return true;
}
bool sphere::bounding_box( aabb& output_box) const {
    output_box = aabb(
        center - glm::vec3(radius, radius, radius),
        center + glm::vec3(radius, radius, radius));
    return true;
}