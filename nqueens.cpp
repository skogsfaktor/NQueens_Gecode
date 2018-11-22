#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

/**
Implementation of nqueens where each field is modelled with one variable, resulting in n*n variables. The size option specifies the
size of the board.
*
Since there are probably no scenarios where a queen is places in the corners, a branching strategy where queens are places in the middle first
would be more suitable. A random strategy could be used for this purpose. 
*
Advantage of using n*n variables: The values of the fields are similar to booleans, which are simpler data structures than integers. 
Disadvantage of using n*n variables: The variables take up O(N^2) variables/space. 
*
Authors: Sadok Dalin & Alexander Barosen
E-mails: sadokd@kth.se & abarosen@kth.se
*/
class NQueens : public Script {
private:
	/// Fields of square
	IntVarArray x;
public:
	/// Post constraints
	NQueens(const SizeOptions& opt)
		: Script(opt), x(*this, opt.size() * opt.size(), 0, 1) {

		Matrix<IntVarArray> m(x, opt.size(), opt.size());

		//Set row constraint 
		for (int i = 0; i < opt.size(); i++) {
			exactly(*this, m.row(i), 1, 1);
		}

		//Set column constraint 
		for (int i = 0; i < opt.size(); i++) {
			exactly(*this, m.col(i), 1, 1);
		}

		IntVarArgs d1y(*this, opt.size(), 0, 0);

		//NW -> SE, lower half 
		for (int i = 0; i < opt.size(); i++) {
			IntVarArgs d1y(*this, opt.size(), 0, 0);
			for (int j = 0; j < opt.size() - i; j++) {
				d1y[j] = m(i + j, j);
			}
			atmost(*this, d1y, 1, 1);
		}

		//NW -> SE, upper half 
		for (int i = 1; i < opt.size(); i++) {
			IntVarArgs d1y(*this, opt.size(), 0, 0);
			for (int j = 0; j < opt.size() - i; j++) {
				d1y[j] = m(j, j + i);
			}
			atmost(*this, d1y, 1, 1);
		}

		//SW -> NE, upper half 
		for (int i = opt.size() - 2; i >= 0; i--) {
			IntVarArgs d1y(*this, opt.size(), 0, 0);
			for (int j = 0; j < i + 1; j++) {
				d1y[j] = m(i - j, j);
			}
			atmost(*this, d1y, 1, 1);
		}

		//SW -> NE, upper half 
		for (int i = 0; i < opt.size(); i++) {
			IntVarArgs d1y(*this, opt.size(), 0, 0);
			for (int j = opt.size() - 1; j >= i; j--) {
				d1y[j] = m(i + opt.size() - 1 - j, j);
			}
			atmost(*this, d1y, 1, 1);
		}

		//Set branching strategy
		Rnd r(1U);
		branch(*this, x, INT_VAR_RND(r), INT_VAL_RND(r));
	}

	/// Constructor for cloning \a s
	NQueens(bool share, NQueens& s) : Script(share, s) {
		x.update(*this, share, s.x);
	}

	/// Copy during cloning
	virtual Space*
		copy(bool share) {
		return new NQueens(share, *this);
	}
	/// Print solution
	virtual void print(std::ostream& os) const {
		//// Matrix-wrapper for the square
		int matrixSize = pow(x.size(), 0.5);
		Matrix<IntVarArray> m(x, matrixSize, matrixSize);
		for (int i = 0; i < matrixSize; i++) {
			os << "\t";
			for (int j = 0; j < matrixSize; j++) {
				os.width(2);
				//flipped
				os << m(i, j) << "  ";
			}
			os << std::endl;
		}
	}
};

int main(int argc, char* argv[]) {
	SizeOptions opt("NQueens");
	opt.size(9);
	opt.parse(argc, argv);
	Script::run<NQueens, DFS, SizeOptions>(opt);
	return 0;
}

