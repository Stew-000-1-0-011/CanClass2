#pragma once

#include "hal_can.hpp"

#include "utility.hpp"

namespace CRSLib::Can
{
	struct Filter

	// 多分bxCAN全体のInitialization ModeとかNomal ModeとかSleep Modeとは独立していると思う.
	// しかし, STM32f103系のリファレンスに(相当に疑わしいのだが)一部操作は「Nomal Mode前に」行う必要があると書かれていた.
	// そもそもなんたらMode中にじゃなくNomal Mode前にってなんだよ.
	// とりあえずHALライブラリのHAL_ConfigFilterやリファレンスの他の記述を見て, bxCANのModeとは独立して実行できると仮定して書く.
	// あとCAN_FMR内のCAN2SBの変更はきっと何らかの制約が伴う(FINITが立ってないとダメとか)と思うのだけど, これの説明はどこにもない.
	// 
	// FINITを立てないと変更ができないレジスタの変更を行う.
	template<CanX can_x>
	HAL_StatusTypeDef cofig_filter_bank(CAN_HandleTypeDef *hcan, const u8 can2_start, const )
	{
		uint32_t filternbrbitpos;
		CAN_TypeDef *can_ip = hcan->Instance;
		HAL_CAN_StateTypeDef state = hcan->State;

		if ((state == HAL_CAN_STATE_READY) ||
			(state == HAL_CAN_STATE_LISTENING))
		{
		#if   defined(CAN2)
			/* CAN1 and CAN2 are dual instances with 28 common filters banks */
			/* Select master instance to access the filter banks */
			can_ip = CAN1;
		#else
			/* CAN1 is single instance with 14 dedicated filters banks */
		#endif

			/* Initialisation mode for the filter */
			SET_BIT(can_ip->FMR, CAN_FMR_FINIT);

		#if   defined(CAN2)
			/* Select the start filter number of CAN2 slave instance */
			CLEAR_BIT(can_ip->FMR, CAN_FMR_CAN2SB);
			SET_BIT(can_ip->FMR, sFilterConfig->SlaveStartFilterBank << CAN_FMR_CAN2SB_Pos);

		#endif
			/* Convert filter number into bit position */
			filternbrbitpos = (uint32_t)1 << (sFilterConfig->FilterBank & 0x1FU);

			/* Filter Deactivation */
			CLEAR_BIT(can_ip->FA1R, filternbrbitpos);

			/* Filter Scale */
			if (sFilterConfig->FilterScale == CAN_FILTERSCALE_16BIT)
			{
			/* 16-bit scale for the filter */
			CLEAR_BIT(can_ip->FS1R, filternbrbitpos);

			/* First 16-bit identifier and First 16-bit mask */
			/* Or First 16-bit identifier and Second 16-bit identifier */
			can_ip->sFilterRegister[sFilterConfig->FilterBank].FR1 =
				((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdLow) << 16U) |
				(0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdLow);

			/* Second 16-bit identifier and Second 16-bit mask */
			/* Or Third 16-bit identifier and Fourth 16-bit identifier */
			can_ip->sFilterRegister[sFilterConfig->FilterBank].FR2 =
				((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdHigh) << 16U) |
				(0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdHigh);
			}

			if (sFilterConfig->FilterScale == CAN_FILTERSCALE_32BIT)
			{
			/* 32-bit scale for the filter */
			SET_BIT(can_ip->FS1R, filternbrbitpos);

			/* 32-bit identifier or First 32-bit identifier */
			can_ip->sFilterRegister[sFilterConfig->FilterBank].FR1 =
				((0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdHigh) << 16U) |
				(0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdLow);

			/* 32-bit mask or Second 32-bit identifier */
			can_ip->sFilterRegister[sFilterConfig->FilterBank].FR2 =
				((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdHigh) << 16U) |
				(0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdLow);
			}

			/* Filter Mode */
			if (sFilterConfig->FilterMode == CAN_FILTERMODE_IDMASK)
			{
			/* Id/Mask mode for the filter*/
			CLEAR_BIT(can_ip->FM1R, filternbrbitpos);
			}
			else /* CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdList */
			{
			/* Identifier list mode for the filter*/
			SET_BIT(can_ip->FM1R, filternbrbitpos);
			}

			/* Filter FIFO assignment */
			if (sFilterConfig->FilterFIFOAssignment == CAN_FILTER_FIFO0)
			{
			/* FIFO 0 assignation for the filter */
			CLEAR_BIT(can_ip->FFA1R, filternbrbitpos);
			}
			else
			{
			/* FIFO 1 assignation for the filter */
			SET_BIT(can_ip->FFA1R, filternbrbitpos);
			}

			/* Filter activation */
			if (sFilterConfig->FilterActivation == CAN_FILTER_ENABLE)
			{
			SET_BIT(can_ip->FA1R, filternbrbitpos);
			}

			/* Leave the initialisation mode for the filter */
			CLEAR_BIT(can_ip->FMR, CAN_FMR_FINIT);

			/* Return function status */
			return HAL_OK;
		}
		else
		{
			/* Update error code */
			hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

			return HAL_ERROR;
		}
	}
}