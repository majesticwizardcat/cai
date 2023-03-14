#pragma once

template <typename EvalType, typename Evaluator, EvalType MaxEvaluation, EvalType MinEvaluation> class MinMaxTree;

#include "game/chess-board.h"

#include "unordered_dense.h"

template <typename EvalType, typename Evaluator, EvalType MinEvaluation, EvalType MaxEvaluation> class MinMaxTree {
public:
	typedef ankerl::unordered_dense::map<ChessBoard, EvalType, BoardHasher> MinMaxMemoMap;

	MinMaxTree()
			: m_minEval(MaxEvaluation)
			, m_maxEval(MinEvaluation) { }

	EvalType expand(const ChessBoard& rootPosition, uint8_t depth) {
		const auto expandFromInner = [this](const ChessBoard& currentPosition, uint8_t currentDepth, const auto recFunc) {
			auto it = m_memo.find(currentPosition);
			if (it != m_memo.end()) {
				return it->second;
			}

			Color nextPlayerColor = currentPosition.getNextPlayerColor();
			MovesVector moves;
			currentPosition.getMoves(nextPlayerColor, moves);

			if (currentDepth == 0 || moves.empty()) {
				EvalType eval = Evaluator()(currentPosition, moves);
				m_memo[currentPosition] = eval;
				return eval;
			}
			
			EvalType eval = nextPlayerColor == WHITE ? MinEvaluation : MaxEvaluation;
			for (const auto& m : moves) {
				ChessBoard b(currentPosition);
				b.playMove(m);
				const EvalType newEval = recFunc(b, currentDepth - 1, recFunc);
				if (nextPlayerColor == WHITE) {
					eval = std::max(eval, newEval);
					if (eval > m_minEval) {
						break;
					}
					m_maxEval = std::max(m_maxEval, eval);
				}
				else {
					eval = std::min(eval, newEval);
					if (eval < m_maxEval) {
						break;
					}
					m_minEval = std::min(m_minEval, eval);
				}
			}
			m_memo[currentPosition] = eval;
			return eval;
		};

		return expandFromInner(rootPosition, depth, expandFromInner);
	}

private:
	MinMaxMemoMap m_memo;
	EvalType m_minEval;
	EvalType m_maxEval;
};
