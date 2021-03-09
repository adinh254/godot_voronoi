#ifndef VORONOI_DETAIL_H
#define VORONOI_DETAIL_H

#include <Godot.hpp>

#include <map>
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
			return reinterpret_cast<T *>(godot_alloc(sizeof(T) * n));
		}

		inline void deallocate(T *ptr, size_t) const noexcept {
			godot_free(ptr);
		}
	};

	template<typename K, typename V>
	using map = std::map<K, V, std::less<K>, GodotAllocator<std::pair<const K, V>>>;

	template<typename T>
	using vector = std::vector<T, GodotAllocator<T>>;
}  // namespace voronoi_detail

#endif // !VORONOI_DETAIL_H
