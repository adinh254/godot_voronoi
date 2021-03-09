#include "voronoi_edge.h"

using namespace godot;

Array VoronoiEdge::sites() const {
	Array result;
	result.push_back(_diagram->_sites[_diagram->_sites_by_index.at(_edge->sites[0]->index)]);
	if (_edge->sites[1] != nullptr) {
		const size_t ref_index = _diagram->_sites_by_index.at(_edge->sites[1]->index);
		result.push_back(_diagram->_sites[ref_index]);
	}
	return result;
}

Vector2 VoronoiEdge::start() const {
	return Vector2(_edge->pos[0].x, _edge->pos[0].y);
}

Vector2 VoronoiEdge::end() const {
	return Vector2(_edge->pos[1].x, _edge->pos[1].y);
}

void VoronoiEdge::setDiagram(const Ref<VoronoiDiagram> &diagram) {
	_diagram = diagram;
}

const Ref<VoronoiDiagram> VoronoiEdge::getDiagram() const {
	return _diagram;
}

void VoronoiEdge::_register_methods() {
	register_method("sites", &VoronoiEdge::sites);
	register_method("start", &VoronoiEdge::start);
	register_method("end", &VoronoiEdge::end);
}

void VoronoiEdge::_init() {
}
