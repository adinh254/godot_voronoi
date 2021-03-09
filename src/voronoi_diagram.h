#ifndef VORONOI_DIAGRAM_H
#define VORONOI_DIAGRAM_H

#include "voronoi_detail.h"
#include "voronoi_site.h"
#include "voronoi_edge.h"

#include <Godot.hpp>
#include <jc_voronoi.h>

namespace godot {
	class VoronoiDiagram : public Reference {
		GODOT_CLASS(VoronoiDiagram, Reference)

	public:
		jcv_diagram _diagram;

		voronoi_detail::vector<Ref<VoronoiEdge>> _edges;
		voronoi_detail::vector<Ref<VoronoiSite>> _sites;

		voronoi_detail::map<std::uintptr_t, size_t> _edges_by_address;
		voronoi_detail::map<int, size_t> _sites_by_index;

		VoronoiDiagram() = default;
		~VoronoiDiagram();

		void build_objects();

		Array edges() const;
		Array sites() const;

	protected:
		static void _register_methods();
		void _init();
	};
} // namespace godot

#endif // !VORONOI_DIAGRAM_H