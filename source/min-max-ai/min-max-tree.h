#pragma once

class MinMaxTree;

#include "game/chess-board.h"
#include "min-max-ai/chess-board-evaluator.hpp"

#include "unordered_dense.h"

class MinMaxTree {
public:
	union EvalDepth {
		struct {
			int16_t evaluation;
			uint8_t depth;
		};
		uint64_t evalDepth;
	};

	typedef ankerl::unordered_dense::map<uint64_t, EvalDepth> MinMaxMemoMap;

	MinMaxTree()
			: m_minEval(CHESS_BOARD_MAX_EVALUATION)
			, m_maxEval(CHESS_BOARD_MIN_EVALUATION) { }

	inline int16_t expand(const ChessBoard& rootPosition, uint8_t depth) {
		const auto expandFromInner = [this](const ChessBoard& currentPosition, uint8_t currentDepth, const auto recFunc) {
			auto it = m_memo.find(currentPosition.getHash());
			if (it != m_memo.end() && it->second.depth >= currentDepth) {
				return it->second.evaluation;
			}

			Color nextPlayerColor = currentPosition.getNextPlayerColor();
			MovesVector moves;
			currentPosition.getMoves(nextPlayerColor, moves);

			if (currentDepth == 0 || moves.empty()) {
				EvalDepth evalDepth;
				evalDepth.evaluation = evaluate(currentPosition, moves);
				evalDepth.depth = 0;
				m_memo[currentPosition.getHash()] = evalDepth;
				return evalDepth.evaluation;
			}
			
			int16_t eval = nextPlayerColor == WHITE ? CHESS_BOARD_MIN_EVALUATION : CHESS_BOARD_MAX_EVALUATION;
			if (nextPlayerColor == WHITE) {
				for (const auto& m : moves) {
					ChessBoard b(currentPosition);
					b.playMove(m);
					const int16_t newEval = recFunc(b, currentDepth - 1, recFunc);
					eval = std::max(eval, newEval);
					if (eval > m_minEval) {
						break;
					}
					m_maxEval = std::max(m_maxEval, eval);
				}
			}
			else {
				for (const auto& m : moves) {
					ChessBoard b(currentPosition);
					b.playMove(m);
					const int16_t newEval = recFunc(b, currentDepth - 1, recFunc);
					eval = std::min(eval, newEval);
					if (eval < m_maxEval) {
						break;
					}
					m_minEval = std::min(m_minEval, eval);
				}
			}

			EvalDepth evalDepth;
			evalDepth.evaluation = eval;
			evalDepth.depth = currentDepth;
			m_memo[currentPosition.getHash()] = evalDepth;
			return eval;
		};

		return expandFromInner(rootPosition, depth, expandFromInner);
	}

private:
	MinMaxMemoMap m_memo;
	int16_t m_minEval;
	int16_t m_maxEval;
};
