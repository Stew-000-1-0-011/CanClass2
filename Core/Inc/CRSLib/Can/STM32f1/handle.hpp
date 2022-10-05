#pragma once

#include "hal_can.hpp"

#include "can_x.hpp"

namespace CRSLib::Can::STM32f1::Implement
{
	CAN_HandleTypeDef crslib_default_hcan(CAN_TypeDef * instance) noexcept
	{
		return 
		CAN_HandleTypeDef
		{
			// レジスタのポインタ
			.Instance = instance,

			// HAL_CAN_Initするためだけに使われる. それ以外じゃ使われない. なぜ他と混ぜてしまったのかは不明.
			// ほぼCanClass2(Ryunika先輩作)からコピペしたもの.
			// Prescalerは多分4であってるんじゃないかな...ここだけ実行時に決定するように書かれてたので変更
			.Init =
			CAN_InitTypeDef
			{
				// わからん.
				.Prescaler = 4,
				// Test Modeに変更することもできる.
				.Mode = CAN_MODE_NORMAL,

				// わからん.
				//Sample-Point at: (1+15)/(1+15+2)=88.9% where CAN open states "The location of the sample point must be as close as possible to 87,5 % of the bit time."
				.SyncJumpWidth = CAN_SJW_1TQ,
				.TimeSeg1 = CAN_BS1_15TQ,
				.TimeSeg2 = CAN_BS2_2TQ,

				// datafield8byteの後ろ2byteを時間の送信に割くか
				// 使う場合にはdlcは常に8
				.TimeTriggeredMode = DISABLE,
				// このモード設定に関係なくTECが多くなると自動でBus-Offする. このモードがオンだと自動で復帰する.
				.AutoBusOff = ENABLE,
				// CAN bus activityが検知されると自動でSleep Modeを抜けるかどうか. よくわからんのでオフ.
				.AutoWakeUp = DISABLE,
				// Time Triggered Communication用. よくわからん.
				// DISABLE 自動で再送しない. Time Triggered Communicationできるようになるらしい.
				// ENABLE 自動再送する. 普通はこっち.
				.AutoRetransmission = ENABLE,
				// 受信Fifoがあふれたときにどこのメッセージを消すか.
				// ENABLEで新しくきたやつ, DISABLEで一番古いやつ
				.ReceiveFifoLocked = DISABLE,
				// 送信Mailboxに複数個中身があったときの優先順位.
				// ENABLE Mailboxの添え字順
				// DISABLE IDの若さ順
				.TransmitFifoPriority = DISABLE,
			},

			// この2つはグローバルな状態を管理するためのもの. なぜ同じ引数にまとめてしまったのか.
			
			// HALライブラリの捉え方によるbxCANの状態. 微妙にSTM32f1系のリファレンスとは状態の捉え方が違う.
			.State = HAL_CAN_STATE_READY,
			// エラーコード.
			.ErrorCode = HAL_CAN_ERROR_NONE
		};
	}
}