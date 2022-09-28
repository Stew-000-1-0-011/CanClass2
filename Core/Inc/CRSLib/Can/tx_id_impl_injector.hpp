#pragma once

#include <CRSLib/std_int.hpp>
#include "utility.hpp"
#include "offset_id.hpp"
#include "id_impl_injector_base.hpp"

namespace CRSLib::Can
{
	template<IsOffsetIdsEnum auto offset_id>
	struct TxIdImplInjector;

	namespace Implement::TxIdImplInjectorImp
	{
		template<class T>
		inline constexpr bool is_tx_id_impl_injector = false;

		template<auto offset_id>
		inline constexpr bool is_tx_id_impl_injector<TxIdImplInjector<offset_id>> = true;
	}

	template<class T>
	concept IsTxIdImplInjector = Implement::TxIdImplInjectorImp::is_tx_id_impl_injector<T> && Implement::IdImplInjectorBase<T>;

	template<IsOffsetIdsEnum auto offset_id>
	requires IsTxIdImplInjector<TxIdImplInjector<offset_id>>
	using TxIdImplInjectorAdaptor = TxIdImplInjector<offset_id>;
}
