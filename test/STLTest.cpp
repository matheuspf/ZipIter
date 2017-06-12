#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <numeric>
#include <random>
#include <iostream>

#include "gtest/gtest.h"
#include "ZipIter/ZipIter.h"


namespace
{
	struct STLTest : public ::testing::Test
	{
		STLTest () {}

		virtual ~STLTest () { }

		virtual void SetUp ()
		{
			v = std::vector<int>(n);

			std::iota(v.begin(), v.end(), 0);
			std::iota(u.begin(), u.end(), 0);
		}

		virtual void TearDown () {}


		static constexpr int n = 10;

		std::vector<int> v;
		std::array<int, n> u;
	};





	TEST_F(STLTest, SortEnumerate)
	{
		std::random_shuffle(v.begin(), v.end());

		std::vector<int> aux(n);

		for(int i = 0; i < n; ++i)
			aux[v[i]] = u[i];

		std::sort(ZIP_ALL(v, u));

		for(int i = 0; i < n; ++i)
			EXPECT_EQ(u[i], aux[i]);
	}


	TEST_F(STLTest, SortSum)
	{
		std::random_shuffle(v.begin(), v.end());
		std::random_shuffle(u.begin(), u.end());

		std::sort(ZIP_ALL(v, u), it::unZip([](int v1, int u1, int v2, int u2){ return v1 + u1 < v2 + u2; }));

		for(int i = 0; i < n-1; ++i)
			EXPECT_LE(v[i] + u[i], v[i+1] + u[i+1]);
	}


	TEST_F(STLTest, Accumulate)
	{
		int res = std::accumulate(ZIP_ALL(v, u), 0, it::unZip([](int sum, int x, int y){ return sum + x + y; }));

		EXPECT_EQ(res, 90);
	}


	TEST_F(STLTest, Transform)
	{
		std::vector<int> aux(n);

		std::transform(ZIP_ALL(v, u), aux.begin(), it::unZip([](int x, int y){ return x * y; }));

		for(int i = 0; i < n; ++i)
			EXPECT_EQ(aux[i], v[i] * u[i]);
	}

	TEST_F(STLTest, TransformBoth)
	{
		std::vector<int> auxV(n);
		std::array<int, n> auxU;

		std::transform(ZIP_ALL(v, u), it::zipBegin(auxV, auxU), it::unZip([](int x, int y)
		{
			return std::make_tuple(x + y, x * y);
		}));

		for(int i = 0; i < n; ++i)
		{
			EXPECT_EQ(auxV[i], v[i] + u[i]);
			EXPECT_EQ(auxU[i], v[i] * u[i]);
		}
	}


	TEST_F(STLTest, Reverse)
	{
		std::vector<int> auxV = v;
		std::array<int, n> auxU = u;

		std::reverse(ZIP_ALL(v, u));

		for(int i = 0; i < n; ++i)
		{
			EXPECT_EQ(auxV[i], v[n-i-1]);
			EXPECT_EQ(auxU[i], u[n-i-1]);
		}
	}



} // namespace

