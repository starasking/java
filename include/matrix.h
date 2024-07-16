// Copyright (c) 2024 Xuemei Wang. All rights reserved.

#ifndef GRAPH_SDK_MATRIX_H
#define GRAPH_SDK_MATRIX_H

#include <algorithm>
#include <numeric>
#include <set>
#include <stack>
#include <tuple>
#include <vector>

#include "../include/utils.h"
namespace graph_sdk {

struct RowCom {
    template <typename T>
    bool operator()(const std::vector<T>& lhs,
                    const std::vector<T>& rhs) const {
        assert(lhs.size() == rhs.size());
        for (auto i = 0; i < lhs.size(); ++i) {
            if (lhs[i] < rhs[i])
                return true;
            else if (lhs[i] > rhs[i])
                return false;
        }
        return false;
    }
};

template <typename T>
class Vec {
   private:
    std::vector<T> vec_{};
    std::size_t size_{};

   public:
    Vec() = default;
    explicit Vec(const std::vector<T>& vec) : vec_(vec), size_(vec.size()) {}
    explicit Vec(size_t n) : vec_(std::vector<T>(size_, T{})), size_(n) {}

    bool is_all(T x) const {
        return std::all_of(vec_.begin(), vec_.end(),
                           [&](const auto& elem) { return elem == x; });
    }

    bool is_any(T x) const {
        return std::any_of(vec_.begin(), vec_.end(),
                           [&](const auto& elem) { return elem == x; });
    }

    bool is_none(T x) const { return !is_any(x); }

    template <class UnaryPred>
    bool is_any(UnaryPred p) const {
        return std::any_of(vec_.begin(), vec_.end(),
                           [&](const auto& elem) { return p(elem); });
    }

    template <class UnaryPred>
    bool is_none(UnaryPred p) const {
        return !is_any(p);
    }

    template <class UnaryPred>
    std::multiset<T> collect_element_if(UnaryPred p) {
        std::multiset<T> result;
        std::copy_if(vec_.begin(), vec_.end(),
                     std::inserter(result, result.end()), p);
        return result;
    }

    size_t count(T x) const { return std::count(vec_.begin(), vec_.end(), x); }

    void replace(T x, T y) { std::replace(vec_.begin(), vec_.end(), x, y); }

    template <class UnaryPred>
    std::vector<size_t> find_all(UnaryPred p) {
        std::vector<size_t> result{};
        for (size_t i = 0; i < vec_.size(); ++i)
            if (p(vec_[i])) {
                result.push_back(i);
            }
        return result;
    }
};

template <typename T>
class Matrix {
   private:
    std::vector<std::vector<T>> mat_{};
    size_t rows_{};
    size_t cols_{};

   public:
    Matrix() = default;

    explicit Matrix(const std::vector<std::vector<T>>& matrix) : mat_(matrix) {
        rows_ = matrix.size();
        if (rows_ > 0) {
            cols_ = matrix[0].size();
            std::for_each(matrix.begin(), matrix.end(),
                          [&](const auto& x) { assert(x.size() == cols_); });
        }
    }

    Matrix(size_t rows, size_t cols)
        : rows_(rows),
          cols_(cols),
          mat_(std::vector<std::vector<T>>(rows, std::vector<T>(cols, T{}))) {
    }

    T& operator()(size_t r, size_t c) {
        assert(r < rows_ && c < cols_);
        return mat_[r][c];
    }

    T operator()(size_t r, size_t c) const {
        assert(r < rows_ && c < cols_);
        return mat_[r][c];
    }

    void print() const {
        std::for_each(mat_.begin(), mat_.end(),
                      [](const auto& x) { print_elem(x); });
    }

    void replace(T x, T y) {
        std::for_each(mat_.begin(), mat_.end(), [&](auto& row) {
            std::replace(row.begin(), row.end(), x, y);
        });
    }

    template <class UnaryPred>
    void replace_if(T y, UnaryPred p) {
        std::for_each(mat_.begin(), mat_.end(), [&](auto& row) {
            std::replace_if(row.begin(), row.end(), p, y);
        });
    }

    void row_replace(size_t i, T x, T y) {
        assert(i < rows_);
        std::replace(mat_[i].begin(), mat_[i].end(), x, y);
    }

    void col_replace(size_t j, T x, T y) {
        assert(j < cols_);
        for (auto i = 0; i < rows_; ++i) {
            if (mat_[i][j] == x) mat_[i][j] = y;
        }
    }

    template <class UnaryPred>
    void row_replace_if(size_t i, T y, UnaryPred p) {
        assert(i < rows_);
        std::replace_if(mat_[i].begin(), mat_[i].end(), p, y);
    }

    template <class UnaryPred>
    void col_replace_if(size_t j, T y, UnaryPred p) {
        assert(j < cols_);
        for (auto i = 0; i < rows_; ++i) {
            if (p(mat_[i][j])) mat_[i][j] = y;
        }
    }

    Vec<T> row(size_t r) const {
        assert(r < rows_);
        return Vec<T>(mat_[r]);
    }

    Vec<T> col(size_t c) const {
        assert(c < cols_);
        std::vector<T> result{};
        std::for_each(mat_.begin(), mat_.end(),
                      [&](auto& row) { result.push_back(row[c]); });
        return Vec<T>(result);
    }

    size_t cols() const { return cols_; }

    size_t rows() const { return rows_; }

    bool is_all(T x) const {
        return std::all_of(mat_.begin(), mat_.end(), [&](const auto& r) {
            return std::all_of(r.begin(), r.end(),
                               [&](const auto& elem) { return elem == x; });
        });
    }

    Matrix<T> transpose() const {
        Matrix<T> mt(mat_);
        for (auto i = 0; i < rows_; ++i) {
            for (auto j = 0; j < cols_; ++j)
                //std::swap(mt.mat_[j][i], mt.mat_[i][j]);
                mt.mat_[j][i] = mat_[i][j];
        }
        return mt;
    }

    std::multiset<T> extract_elem_multiset() const {
        std::multiset<T> elem_multiset{};
        std::for_each(mat_.begin(), mat_.end(), [&](const auto& x) {
            std::copy(x.begin(), x.end(),
                      std::inserter(elem_multiset, elem_multiset.end()));
        });
        return elem_multiset;
    }

    Matrix row_cat(const Matrix& m) const {
        assert(rows_ == m.rows_);
        Matrix result(rows_, cols_ + m.cols_);
        for (auto i = 0; i < rows_; ++i) {
            for (auto j = 0; j < result.cols_; ++j) {
                result.mat_[i][j] =
                    (j < cols_) ? mat_[i][j] : m.mat_[i][j - cols_];
            }
        }
        return result;
    }

    std::tuple<std::vector<size_t>, Matrix> row_sort_with_indice() const {
        Matrix m(mat_);
        std::sort(m.mat_.begin(), m.mat_.end(), RowCom());
        RowCom rc;
        std::vector<size_t> idx(rows_);
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(),
                  [&](size_t r1, size_t r2) { return rc(mat_[r1], mat_[r2]); });
        return std::make_tuple(idx, m);
    }

    std::tuple<std::vector<size_t>, Matrix> col_sort_with_indice() const {
        Matrix m(mat_);
        m = m.transpose();
        auto [idx, mc] = m.row_sort_with_indice();
        return std::make_tuple(idx, mc.transpose());
    }

    friend bool operator==(const Matrix& m1, const Matrix& m2) {
        if (m1.cols_ == m2.cols_ && m1.rows_ == m1.rows_) {
            for (auto i = 0; i < m1.rows_; ++i)
                for (auto j = 0; j < m1.cols_; ++j)
                    if (m1.mat_[i][j] != m2.mat_[i][j]) return false;
        } else
            return false;
        return true;
    }

    friend bool operator!=(const Matrix& m1, const Matrix& m2) {
        return !(m1 == m2);
    }

    friend Matrix operator-(const Matrix& m1, const Matrix& m2) {
        assert((m1.cols_ == m2.cols_ && m1.rows_ == m1.rows_));
        Matrix m(m1.rows_, m1.cols_);
        for (auto i = 0; i < m1.rows_; ++i)
            for (auto j = 0; j < m1.cols_; ++j)
                m.mat_[i][j] = m1.mat_[i][j] - m2.mat_[i][j];
        return m;
    }

    friend Matrix operator-(const Matrix& m1, T value) {
        Matrix m(m1.rows_, m1.cols_);
        for (auto i = 0; i < m1.rows_; ++i)
            for (auto j = 0; j < m1.cols_; ++j)
                m.mat_[i][j] = m1.mat_[i][j] - value;
        return m;
    }

    friend Matrix operator-(const Matrix& m1) {
        Matrix m(m1.rows_, m1.cols_);
        for (auto i = 0; i < m.rows_; ++i)
            for (auto j = 0; j < m.cols_; ++j) m.mat_[i][j] = -m1.mat_[i][j];
        return m;
    }
};

}  // namespace graph_sdk
#endif
