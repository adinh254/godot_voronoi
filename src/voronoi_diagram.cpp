#include "voronoi_diagram.h"

using namespace godot;

VoronoiDiagram::~VoronoiDiagram() {
	// Ref wrapper should dereference and delete.
	//for (auto edge : _edges)
	//	godot_object_destroy(&edge);
	//for (auto site : _sites)
	//	godot_object_destroy(static_cast<Object*>(site));

	jcv_diagram_free(&_diagram);
}

Array VoronoiDiagram::edges() const {
	Array result;
	for (const auto &edge : _edges)
		result.push_back(edge);
	return result;
}

Array VoronoiDiagram::sites() const {
	Array result;
	for (const auto &site : _sites)
		result.push_back(site);
	return result;
}

void VoronoiDiagram::build_objects() {
	voronoi_detail::vector<Ref<VoronoiEdge>> gd_edges;
	const jcv_edge *edge = jcv_diagram_get_edges(&_diagram);
	while (edge) {
		// apparent bug in jcv, egdes where start = end are reported as
		// diagram edges, but do not exist when iterating over sites
		if (edge->pos[0].x != edge->pos[1].x || edge->pos[0].y != edge->pos[1].y) {
			// Manual Initialization.
			Ref<VoronoiEdge> gd_edge{ VoronoiEdge::_new() };
			gd_edge->_edge = edge;
			gd_edge->setDiagram(Ref<VoronoiDiagram>{ this });

			_edges_by_address[reinterpret_cast<std::uintptr_t>(edge)] = gd_edges.size();
			gd_edges.push_back(gd_edge);
		}
		edge = edge->next;
	}
	_edges = std::move(gd_edges);

	voronoi_detail::vector<Ref<VoronoiSite>> gd_sites;
	const jcv_site *sites = jcv_diagram_get_sites(&_diagram);
	for (int i = 0; i < _diagram.numsites; i++) {
		// Manual Initialization.
		Ref<VoronoiSite> gd_site{ VoronoiSite::_new() };
		gd_site->_site = &sites[i];
		gd_site->setDiagram(Ref<VoronoiDiagram>{ this });

		_sites_by_index[sites[i].index] = gd_sites.size();
		gd_sites.push_back(gd_site);
	}
	_sites = std::move(gd_sites);
}

void VoronoiDiagram::_register_methods() {
	register_method("edges", &VoronoiDiagram::edges);
	register_method("sites", &VoronoiDiagram::sites);
}

void VoronoiDiagram::_init() {
	_diagram = jcv_diagram{};
}