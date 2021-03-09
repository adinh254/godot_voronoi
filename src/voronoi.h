#ifndef VORONOI_H
#define VORONOI_H

#include <Godot.hpp>
#include <ConvexPolygonShape2D.hpp>
#include <jc_voronoi.h>

#include <vector>

namespace voronoi_detail {
	template<typename T>
	struct GodotAllocator {
		using value_type = T;

		constexpr GodotAllocator() noexcept {}

		template<typename U>
		GodotAllocator(const GodotAllocator<U> &) noexcept {}

		template<typename U>
		bool operator==(const GodotAllocator<U> &) const noexcept {
			return true;
		}

		template<typename U>
		bool operator!=(const GodotAllocator<U> &) const noexcept {
			return false;
		}

		inline T *allocate(size_t n) const {
			return reinterpret_cast<T *>(godot::api->godot_alloc(sizeof(T) * n));
		}

		inline void deallocate(T *ptr, size_t) const noexcept {
			godot::api->godot_free(ptr);
		}
	};

	template<typename T>
	using vector = std::vector<T, GodotAllocator<T>>;
}  // namespace voronoi_detail


namespace godot {
	class VoronoiDiagram : public Reference {
		GODOT_CLASS(VoronoiDiagram, Reference)

	public:
		static void _register_methods();
		void _init();

		PoolVector2Array centers() const;
		Vector2 center_at(int site_idx) const;

		Array cells() const;
		PoolVector2Array cell_at(int site_idx) const;

		Array polygons() const;
		Ref<ConvexPolygonShape2D> polygon_at(int site_idx) const;

		PoolIntArray neighbors_at(int site_idx) const;

		VoronoiDiagram() = default;
		~VoronoiDiagram();

		void set_diagram(const jcv_diagram &diagram);
	private:
		jcv_diagram _diagram;
	};

	class Voronoi : public Reference {
		GODOT_CLASS(Voronoi, Reference)

	public:
		static void _register_methods();
		void _init();

		void set_points(PoolVector2Array points);
		void set_boundaries(Rect2 boundaries);
		void relax_points(int iterations);
		Ref<VoronoiDiagram> generate_diagram() const;

		Voronoi() = default;
		~Voronoi() = default;
	
	private:
		jcv_rect _boundaries;
		bool _has_boundaries;
		voronoi_detail::vector<jcv_point> _points;
	};
} // namespace godot

#endif // !VORONOI_H
