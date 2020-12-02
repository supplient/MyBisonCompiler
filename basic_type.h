#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

namespace rtex {

    //
    // Small Types
    // 
    typedef double Real;
    typedef int Integer;

    //
    // Errors
    //

    class MatrixDimensionNotMatchError {};
    class MatrixVoidDimensionError {};
    class MatrixNotSquareError {};

    //
    // Matrix Defination
    //

    class Matrix {
    public:
        Matrix(int n):Matrix(n, n) {}

        Matrix(int n, int m):n(n), m(m) {
            a.resize(n);
            for(auto& row: a)
                row.resize(m);
        }

        Matrix(std::initializer_list<std::initializer_list<Real>> args) {
            n = args.size();
            a.resize(n);

            auto rowIter = args.begin();
            m = (*rowIter).size();
            for(auto& row: a) {
                const auto& subArgs = *rowIter;
                if(subArgs.size() != m)
                    throw MatrixDimensionNotMatchError();
                row.resize(m);

                auto colIter = subArgs.begin();
                for(int j=0; j<m; j++) {
                    row[j] = *colIter;
                    colIter++;
                }

                rowIter++;
            }
        }

        Matrix(const std::vector<std::vector<Real>>& b) {
            n = b.size();
            if(n < 1)
                throw MatrixVoidDimensionError();
            m = b[0].size();
            a = b;
        }

        Matrix(const Matrix& mat) {
            *this = mat.a;
            n = mat.n;
            m = mat.m;
        }

        int GetN()const { return n; }
        int GetM()const { return m; }
        void ResetN(int newN) {
            n = newN;
        }
        void ResetM(int newM) {
            m = newM;
        }
        void ResetSize(int newN, int newM=-1) {
            ResetN(newN);
            if(newM == -1)
                newM = newN;
            ResetM(newM);
        }

        void SetIdentity(Real k=1) {
            for(auto pair: *this) {
                int i = pair.first;
                int j = pair.second;
                if(i==j)
                    (*this)[i][j] = k;
                else
                    (*this)[i][j] = 0;
            }
        }

        Matrix Transpose()const {
            Matrix res(m, n);
            for(auto pair: *this) {
                int i = pair.first;
                int j = pair.second;
                res[j][i] = (*this)[i][j];
            }
            return res;
        }

        void SubIdentity(Real k) {
            if(n != m)
                throw MatrixNotSquareError();
            for(int i=0; i<n; i++) {
                (*this)[i][i] -= k;
            }
        }

        Matrix& operator=(const std::vector<std::vector<Real>>& b) {
            a = b;
            return *this;
        }


        class ColSelector {
        public:
            ColSelector(Matrix& m):m(m){}
            ColSelector(const ColSelector& b):ColSelector(b.m){}

            class ColOperator {
            public:
                ColOperator(Matrix& m, int col): m(m), col(col) {}
                ColOperator(const ColOperator& b):ColOperator(b.m, b.col) {}
                ColOperator& operator=(const std::vector<Real>& a) {
                    if(m.GetN() != a.size())
                        throw MatrixDimensionNotMatchError();
                    for(int i=0; i<a.size(); i++)
                        m[i][col] = a[i];
                    return *this;
                }
            private:
                Matrix& m;
                int col;
            };

            ColOperator operator[](int col) {
                return ColOperator(m, col);
            }

        private:
            Matrix& m;
        };

        enum Mark {
            all
        };

        std::vector<Real>& operator[](int i) {
            return a[i];
        }
        std::vector<Real> operator[](int i)const {
            return a[i];
        }
        ColSelector operator[](Mark m) {
            switch (m)
            {
            case all:
                return ColSelector(*this);
            default:
                throw "Invalid mark";
            }
        }


        Matrix operator*(const Matrix& v) {
            if(m != v.n)
                throw MatrixDimensionNotMatchError();
            Matrix res(n, v.m);
            for(auto pair: res) {
                int i = pair.first;
                int j = pair.second;

                Real sum = 0;
                for(int k=0; k<m; k++)
                    sum += (*this)[i][k] * v[k][j];
                res[i][j] = sum;
            }
            return res;
        }

        void operator+=(const Matrix& v) {
            if(n != v.n || m != v.m)
                throw MatrixDimensionNotMatchError();
            for(auto pair: *this) {
                int i = pair.first;
                int j = pair.second;
                
                (*this)[i][j] += v[i][j];
            }
        }

        void operator-=(const Matrix& v) {
            if(n != v.n || m != v.m)
                throw MatrixDimensionNotMatchError();
            for(auto pair: *this) {
                int i = pair.first;
                int j = pair.second;
                
                (*this)[i][j] -= v[i][j];
            }
        }

        struct Iterator {
            int i;
            int j;
            const Matrix& mat;

            Iterator operator++() {
                Iterator res = *this;
                j++;
                if(j >= mat.m) {
                    j = 0;
                    i++;
                }
                return res;
            }
            std::pair<int, int> operator*() {
                return {i, j};
            }
            bool operator!=(const Iterator& b) {
                return i!=b.i || j!=b.j;
            }
        };

        Iterator begin() const{
            return {0, 0, *this};
        }

        Iterator end() const{
            return {n, 0, *this};
        }

        static void Test();

        std::vector<std::vector<Real>> a;
        int n,m;
    };

    template<class T>
    T& operator<<(T& out, const Matrix& m) {
        for(int i=0; i<m.n; i++) {
            out << "[";
            for(int j=0; j<m.m; j++) {
                out << m[i][j];
                if(j != m.m-1)
                    out << ", ";
            }
            out << "]";
            if(i != m.n-1)
                out << ",\n";
        }

        return out;
    }

    Matrix operator*(Real k, const Matrix& m);

}

#endif //MATRIX_H