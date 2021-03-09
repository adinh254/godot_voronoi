#ifndef VORONOI_EDGE_H
#define VORONOI_EDGE_H

#include "voronoi_diagram.h"

#include <Godot.hpp>
#include <jc_voronoi.h>

namespace godot {
	class VoronoiEdge : public Object {
		GODOT_CLASS(VoronoiEdge, Object)

	public:
		const jcv_edge *_edge;

		VoronoiEdge() = default;
		~VoronoiEdge() = default;

		Array sites() const;
		Vector2 start() const;
		Vector2 end() const;

		void setDiagram(const Ref<VoronoiDiagram> &diagram);
		const Ref<VoronoiDiagram> getDiagram() const;

	protected:
		static void _register_methods();
		void _init();

	private:
		Ref<VoronoiDiagram> _diagram;
	};
} // namespace godot

#endif // !VORONOI_EDGE_H