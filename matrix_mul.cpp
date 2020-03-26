#include <hpx/include/iostreams.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/util.hpp>
#include <hpx/include/parallel_generate.hpp>

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

std::int64_t rand_wrapper()
{
    return std::rand()%10000;
}

std::int64_t vector_multiply(std::vector<std::int64_t> a,std::vector<std::int64_t> b)
{
    std::int64_t sum=0;
    for(int i=0;i<a.size();i++)
    {
        sum+=a[i]*b[i];
    }
    return sum;
}

//[hpx_main
int hpx_main(hpx::program_options::variables_map& vm)
{
    // extract command line argument, i.e. fib(N)
    int n = vm["n"].as<int>();
    int m = vm["m"].as<int>();
    int is_unit = vm["is_unit"].as<int>();

    //init the Matrices 1 and 2
    std::vector<std::vector<std::int64_t> > a,b;
    a.resize(n);b.resize(m);

    //NOTE: A is stored in row major order 
    //NOTE: B is stored in column major order 

    //random init for matrix A
    for(int i=0;i<n;i++)
    {
        a[i].resize(m);
        hpx::parallel::generate(hpx::parallel::execution::par,
            std::begin(a[i]), std::end(a[i]), &rand_wrapper);

    }

    //print A
    hpx::cout<<"\nA is"<<"\n";
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
        {
            hpx::cout<<a[i][j]<<" ";
        }
        hpx::cout<<"\n";
    }

    //unit/random matrix B
    for(int i=0;i<m;i++)
    {
        b[i].resize(m);
        if(!is_unit)
        hpx::parallel::generate(hpx::parallel::execution::par,
            std::begin(b[i]), std::end(b[i]), &rand_wrapper);
    }

    if(is_unit)
    {
        for(int i=0;i<m;i++)
        {
            for(int j=0;j<m;j++)
            {
                if(i == j) b[i][j]=1;
                else  b[i][j]=0;
            }
        }
    }
    else
    {
        //print B
        hpx::cout<<"\nB is: "<<"\n";
        for(int i=0;i<m;i++)
        {
            for(int j=0;j<m;j++)
            {
                hpx::cout<<b[j][i]<<" ";
            }
            hpx::cout<<"\n";
        }
    }

    std::vector<hpx::future<std::int64_t> > temp;
    temp.resize(n*m);

    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
        {
            temp[i*m+j]=hpx::async(vector_multiply,a[i],b[j]);
        }
    }

    auto an=hpx::util::unwrap(temp);
    hpx::cout<<"\nMultiplied matrix is :\n";
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
        {
            hpx::cout<<an[i*m+j]<<" ";
        }
        hpx::cout<<"\n";
    }

    return hpx::finalize(); // Handles HPX shutdown
}
//hpx_main]

//[main
int main(int argc, char* argv[])
{
    std::cout<<"Program multiples random matrix A with unit/random matrix B to help verify correctness of program easily\n";
    // Configure application-specific options
    hpx::program_options::options_description
       desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
        ( "n",
          hpx::program_options::value<int>()->default_value(5),
          "Number of rows in Matrix 1")
        ( "m",
          hpx::program_options::value<int>()->default_value(5),
          "Number of columns in Matrix 2")
        ( "is_unit",
          hpx::program_options::value<int>()->default_value(1),
          "Is Matrix 2 unit")
          ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
//main]