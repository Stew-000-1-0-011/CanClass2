//#include <can_manager.hpp>
//#include <tuple>
//
//using namespace CRSLib::Can;
//
//enum class CanCommonCommands : CRSLib::u32
//{
//	emergency_stop,
//	n
//};
//
//enum class HogeCommands : CRSLib::u32
//{
//	hoge,
//	fuga,
//	n
//};
//
//enum class Hoge2Commands : CRSLib::u32
//{
//	hoge,
//	fuga,
//	n
//};
//
//namespace CRSLib::Can{
//	template<class ... UElements>
//	void h(UElements&& ...)
//	{
//	}
//
//	void f(CAN_HandleTypeDef *const hcan)
//	{
//		using namespace CRSLib::IntegerLiterals;
//
//
//
//		  [[maybe_unused]] auto can_manager = make_can_manager<CanX::can1, 14_u32>(hcan, std::tuple{CRSLib::ToValue<CanCommonCommands>(), 0_u32, FifoIndex::fifo0}, std::tuple{CRSLib::ToValue<HogeCommands>(), 4_u32, FifoIndex::fifo1}, std::tuple{CRSLib::ToValue<Hoge2Commands>(), 4_u32, FifoIndex::fifo1});
////		std::tuple<CanUnit<OffsetIdsEnums, int> ...> hoge{std::tuple{make_can_unit<OffsetIdsEnums>(std::get<1>(args), (int *)nullptr) ...}};
//	}
//}
//
//void g()
//{
//	using namespace CRSLib;
//	using namespace CRSLib::Can;
//	using namespace CRSLib::IntegerLiterals;
//
//	f(nullptr);
//}

//#include <stdint.h>
//#include <cmsis_armcc.h>
//
//void f()
//{
//	uint32_t a;
//	auto x = __LDREXW(&a);
//}
