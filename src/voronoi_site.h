#ifndef VORONOI_SITE_H
#define VORONOI_SITE_H

#include <Godot.hpp>
#include <jc_voronoi.h>


namespace godot {
	class VoronoiSite : public Object {
		GODOT_CLASS(VoronoiSite, Object)

	public:
		const jcv_site *_site;

		VoronoiSite() = default;
		~VoronoiSite() = default;

		int index() const;
		Vector2 center() const;
		Array edges() const;
		Array neighbors() const;

		void setDiagram(const Ref<VoronoiDiagram> &diagram);
		const Ref<VoronoiDiagram> getDiagram() const;

	protected:
		static void _register_methods();
		void _init();

	private:
		Ref<VoronoiDiagram> _diagram;
	};
} // namespace godot

#endif // !VORONOI_SITE_H