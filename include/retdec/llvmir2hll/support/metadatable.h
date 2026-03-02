/**
* @file include/retdec/llvmir2hll/support/metadatable.h
* @brief A mixin providing metadata attached to objects.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_LLVMIR2HLL_SUPPORT_METADATABLE_H
#define RETDEC_LLVMIR2HLL_SUPPORT_METADATABLE_H

#include <memory>
#include <utility>

namespace retdec {
namespace llvmir2hll {

/**
* @brief A mixin providing metadata attached to objects.
*
* @tparam T Type of metadata.
*
* Uses lazy allocation: the metadata storage is only allocated when
* non-empty metadata is actually set, saving memory per IR node.
*/
template<typename T>
class Metadatable {
public:
	/**
	* @brief Attaches new metadata.
	*
	* @param[in] newData Metadata to be attached.
	*/
	void setMetadata(T newData) {
		if (!data) {
			data = std::make_unique<T>(std::move(newData));
		} else {
			*data = std::move(newData);
		}
	}

	/**
	* @brief Returns the attached metadata.
	*/
	T getMetadata() const {
		return data ? *data : T{};
	}

	/**
	* @brief Are there any non-empty metadata?
	*/
	bool hasMetadata() const {
		return data && !data->empty();
	}

protected:
	/**
	* @brief Constructs a new metadatable object.
	*/
	Metadatable() = default;

private:
	/// Attached metadata (lazily allocated).
	std::unique_ptr<T> data;
};

} // namespace llvmir2hll
} // namespace retdec

#endif
