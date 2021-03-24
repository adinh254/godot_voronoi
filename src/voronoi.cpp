#define JC_VORONOI_IMPLEMENTATION
#include "voronoi.h"

#include <PoolArrays.hpp>

namespace {
	void *useralloc(void *ctx, size_t size) {
		return godot::api->godot_alloc(size);
	}

	void userfree(void *ctx, void *ptr) {
		return godot::api->godot_free(ptr);
	}
} // namespace 

using namespace godot;

void VoronoiDiagram::_register_methods() {
	register_method("get_site_count", &VoronoiDiagram::get_site_count);
	register_method("get_site_positions", &VoronoiDiagram::get_site_positions);
	register_method("get_cells", &VoronoiDiagram::get_cells);
	register_method("get_polygons", &VoronoiDiagram::get_polygons);
	register_method("site_get_position", &VoronoiDiagram::site_get_position);
	register_method("site_get_cell", &VoronoiDiagram::site_get_cell);
	register_method("site_get_polygon", &VoronoiDiagram::site_get_polygon);
	register_method("site_get_neighbors", &VoronoiDiagram::site_get_neighbors);
}

void VoronoiDiagram::_init() {
	_diagram = jcv_diagram{};
}

int VoronoiDiagram::get_site_count() const {
	return _diagram.numsites;
}

PoolVector2Array VoronoiDiagram::get_site_positions() const {
	PoolVector2Array result{};
	result.resize(_diagram.numsites);

	const jcv_site *sites = jcv_diagram_get_sites(&_diagram);
	PoolVector2Array::Write result_w = result.write();
	for (int i = 0; i < _diagram.numsites; i++) {
		result_w[i] = Vector2{ sites[i].p.x, sites[i].p.y };
	}

	return result;
}

Array VoronoiDiagram::get_cells() const {
	Array result{};
	result.resize(_diagram.numsites);

	const jcv_site *sites = jcv_diagram_get_sites(&_diagram);
	for (int i = 0; i < _diagram.numsites; i++) {
		result[i] = site_get_cell(i);
	}
	return result;
}

Array VoronoiDiagram::get_polygons() const {
	Array result{};
	result.resize(_diagram.numsites);

	for (int i = 0; i < _diagram.numsites; i++) {
		result[i] = site_get_polygon(i);
	}
	return result;
}

Vector2 VoronoiDiagram::site_get_position(const int site_id) const {
	const jcv_site *sites = jcv_diagram_get_sites(&_diagram);
	return Vector2{ sites[site_id].p.x, sites[site_id].p.y };
}

PoolVector2Array VoronoiDiagram::site_get_cell(const int site_id) const {
	PoolVector2Array result{};

	const jcv_site *sites = jcv_diagram_get_sites(&_diagram);
	const jcv_graphedge *graphedge = sites[site_id].edges;
	while (graphedge) {
		result.push_back(Vector2{ graphedge->pos[0].x, graphedge->pos[0].y });
		graphedge = graphedge->next;
	}
	return result;
}

Ref<ConvexPolygonShape2D> VoronoiDiagram::site_get_polygon(const int site_id) const {
	Ref<ConvexPolygonShape2D> cell_polygon{};
	cell_polygon.instance();
	cell_polygon->set_points(site_get_cell(site_id));
	return cell_polygon;
}

Array VoronoiDiagram::site_get_neighbors(const int site_id) const {
	Array result{};

	const jcv_site *sites = jcv_diagram_get_sites(&_diagram);
	const jcv_graphedge *graphedge = sites[site_id].edges;
	while (graphedge) {
		if (graphedge->neighbor)
			result.push_back(graphedge->neighbor->index);
		graphedge = graphedge->next;
	}
	return result;
}

VoronoiDiagram::~VoronoiDiagram() {
	jcv_diagram_free(&_diagram);
}

void VoronoiDiagram::set_diagram(const jcv_diagram &diagram) {
	_diagram = diagram;
}

void Voronoi::_register_methods() {
	register_method("set_points", &Voronoi::set_points);
	register_method("set_boundaries", &Voronoi::set_boundaries);
	register_method("relax_points", &Voronoi::relax_points);
	register_method("generate_diagram", &Voronoi::generate_diagram);
}

void Voronoi::_init() {
}

void Voronoi::set_points(PoolVector2Array points) {
	assert(points.size());

	// translate Godot Vector2 points into jcv_points
	voronoi_detail::vector<jcv_point> new_points{};
	for (int i = 0; i < points.size(); i++)
		new_points.push_back({ points[i].x, points[i].y });

	_points = std::move(new_points);
}

void Voronoi::set_boundaries(Rect2 boundaries) {
	_boundaries = jcv_rect {
		jcv_point { boundaries.position.x, boundaries.position.y },
		jcv_point { boundaries.position.x + boundaries.size.x, boundaries.position.y + boundaries.size.y }
	};
	_has_boundaries = true;
}

void Voronoi::relax_points(int iterations = 1) {
	voronoi_detail::vector<jcv_point> new_points{};
	for (int j = 0; j < iterations; j++) {
		jcv_diagram diagram;
		memset(&diagram, 0, sizeof(jcv_diagram));
		jcv_diagram_generate_useralloc(
			_points.size(),
			_points.data(),
			_has_boundaries ? &_boundaries : NULL,
			NULL,
			NULL,
			&useralloc,
			&userfree,
			&diagram
		);
		const jcv_site* sites = jcv_diagram_get_sites(&diagram);
		const int numsites = diagram.numsites;
		for (int i = 0; i < numsites; ++i) {
			const jcv_site* site = &sites[i];
			jcv_point sum = site->p;
			int count = 1;

			const jcv_graphedge* edge = site->edges;

			while (edge) {
				sum.x += edge->pos[0].x;
				sum.y += edge->pos[0].y;
				++count;
				edge = edge->next;
			}

			new_points.push_back({ sum.x / count, sum.y / count });
		}
		jcv_diagram_free(&diagram);
	}
	_points = std::move(new_points);
}

Ref<VoronoiDiagram> Voronoi::generate_diagram() const {
	Ref<VoronoiDiagram> result{};
	result.instance();

	jcv_diagram new_diagram{};
	jcv_diagram_generate_useralloc(
		_points.size(),
		_points.data(),
		_has_boundaries ? &_boundaries : NULL,
		NULL,
		NULL,
		&useralloc,
		&userfree,
		&new_diagram
	);
	result->set_diagram(new_diagram);
	return result;
}