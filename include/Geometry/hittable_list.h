//#ifndef HITTABLE_LIST_H
//#define HITTABLE_LIST_H
//#include <memory>
//#include <vector>
//
//#include <Geometry/hittable.h>
//
//using std::shared_ptr;
//using std::make_shared;
//
//aabb surrounding_box(aabb box0, aabb box1);
//
//class hittable_list : public hittable {
//public:
//	hittable_list() {}
//	hittable_list(shared_ptr<hittable> object) { add(object); }
//
//	void clear() { objects.clear(); }
//	void add(shared_ptr<hittable> object);
//
//	virtual bool hit(const Ray& r, SurfaceInteraction& rec) const override;
//	bool hit(const Ray& r) const;
//
//	virtual bool bounding_box(float t0, float t1, aabb& output_box) const override;
//	float pdf_value(const Point3f& o, const Vector3f& v) const override;
//	Vector3f random(const Point3f& o) const override;
//
//	std::vector<shared_ptr<hittable>> objects;
//	std::vector<shared_ptr<hittable>> pdf_objects;
//};
//
//using Scene = hittable_list;
//
//#endif
