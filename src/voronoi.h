#ifndef VORONOI_H
#define VORONOI_H

#include <Godot.hpp>
#include <ConvexPolygonShape2D.hpp>
#include <jc_voronoi.h>

#include <vector>
#include<unordered_map>

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

	template<typename K, typename V>
	using map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, GodotAllocator<std::pair<const K, V>>>;

	template<typename T>
	using vector = std::vector<T, GodotAllocator<T>>;
}  // namespace voronoi_detail


namespace godot {
	class VoronoiDiagram : public Reference {
		GODOT_CLASS(VoronoiDiagram, Reference)

	public:
		static void _register_methods();
		void _init();

		int get_site_count() const;
		PoolVector2Array get_site_positions() const;
		Array get_cells() const;
		Array get_polygons() const;

		Vector2 site_get_position(int site_id) const;
		PoolVector2Array site_get_cell(int site_id) const;
		Ref<ConvexPolygonShape2D> site_get_polygon(int site_id) const;
		Array site_get_neighbors(int site_id) const;

		VoronoiDiagram() = default;
		~VoronoiDiagram();

		void set_diagram(const jcv_diagram &diagram);

		void build_objects();
	private:
		jcv_diagram _diagram;
		voronoi_detail::map<int, int> _sites_by_index;
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