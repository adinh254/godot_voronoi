#include "voronoi_site.h"

using namespace godot;

int VoronoiSite::index() const {
	return _site->index;
}

Vector2 VoronoiSite::center() const {
	return Vector2(_site->p.x, _site->p.y);
}

Array VoronoiSite::edges() const {
	Array result;
	const jcv_graphedge *graphedge = _site->edges;
	while (graphedge) {
		const size_t ref_index = _diagram->_edges_by_address.at(
			reinterpret_cast<std::uintptr_t>(graphedge->edge));
		result.push_back(_diagram->_edges[ref_index]);
		graphedge = graphedge->next;
	}
	return result;
}

Array VoronoiSite::neighbors() const {
	Array result;
	const jcv_graphedge *graphedge = _site->edges;
	while (graphedge) {
		if (graphedge->neighbor) {
			const size_t site_index = _diagram->_sites_by_index.at(graphedge->neighbor->index);
			result.push_back(_diagram->_edges[site_index]);
		}
		graphedge = graphedge->next;
	}
	return result;
}

void VoronoiSite::setDiagram(const Ref<VoronoiDiagram> &diagram) {
	_diagram = diagram;
}

const Ref<VoronoiDiagram> VoronoiSite::getDiagram() const {
	return _diagram;
}

void VoronoiSite::_register_methods() {
	register_method("index", &VoronoiSite::index);
	register_method("center", &VoronoiSite::center);
	register_method("edges", &VoronoiSite::edges);
	register_method("neighbors", &VoronoiSite::neighbors);
}

void VoronoiSite::_init() {
}
