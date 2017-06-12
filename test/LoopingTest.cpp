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
	struct LoopingTest : public ::testing::Test
	{
		LoopingTest () { }

		virtual ~LoopingTest () { }


		virtual void SetUp ()
		{
			gen.seed(gen());
			
			n = rng(10, 50);

			a = new int[n];

			for(int i = 0; i < n; ++i)
			{
				v.push_back(rng(0, 100));
				l.push_back(rng(0, 100));
				s.insert(rng(0, 100));
				a[i] = rng(0, 100);
			}


			auto itv = v.begin();
			auto itl = l.begin();
			auto its = s.begin();
			auto ita = a;
			auto ite = v.end();

			for(; itv != ite; ++itv, ++itl, ++its, ++ita)
				insertBack(sequence, *itv, *itl, *its, *ita);
		}


		virtual void TearDown ()
		{
			delete[] a;
		}


		int rng (int l, int u)
		{
			return std::uniform_int_distribution<>(l, u - 1)(gen);
		}


		static void insertBack (std::vector<int>& r, int x, int y, int w, int z)
		{
			r.push_back(x);
        	r.push_back(y);
        	r.push_back(w);
        	r.push_back(z);
		}



		int n;

		std::vector<int> v;
		std::list<int>   l;
		std::set<int>    s;
		int*             a;

		std::vector<int> sequence;
		std::vector<int> res;


		std::random_device dvc;
		std::mt19937 	   gen;
	};






	TEST_F(LoopingTest, ForEachStd)
	{
		std::for_each(ZIP_ALL(v, l, s, a), [&](auto tup)
		{
			insertBack(res, std::get<0>(tup), std::get<1>(tup), std::get<2>(tup), std::get<3>(tup));
		});

    	EXPECT_EQ(res, sequence);
	}


	TEST_F(LoopingTest, ForEachStdUnzip)
	{
		std::for_each(ZIP_ALL(v, l, s, a), it::unZip([&](auto x, auto y, auto w, auto z)
        {
        	insertBack(res, x, y, w, z);
    	}));

    	EXPECT_EQ(res, sequence);
	}




	TEST_F(LoopingTest, ForRange)
	{
		for(auto tup : it::zip(v, l, s, a))
        	insertBack(res, std::get<0>(tup), std::get<1>(tup), std::get<2>(tup), std::get<3>(tup));

    	EXPECT_EQ(res, sequence);
	}


	TEST_F(LoopingTest, ForRangeUnzip)
	{
		for(auto tup : it::zip(v, l, s, a)) it::unZip(tup, [&](auto x, auto y, auto w, auto z)
		{
        	insertBack(res, x, y, w, z);
    	});

    	EXPECT_EQ(res, sequence);
	}


	TEST_F(LoopingTest, ForEachFunc)
	{
	    it::forEach(v, l, s, a, [&](auto x, auto y, auto w, auto z)
	    {
	        insertBack(res, x, y, w, z);
	    });

    	EXPECT_EQ(res, sequence);
	}

} // namespace

