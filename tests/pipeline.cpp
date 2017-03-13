/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                     Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *
 * This file is part of Tuyau <https://github.com/bilgili/Tuyau>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define BOOST_TEST_MODULE Pipeline

#include <tuyau/pipeFilter.h>
#include <tuyau/filter.h>
#include <tuyau/pipeline.h>
#include <tuyau/pushExecutor.h>
#include <tuyau/workers.h>
#include <tuyau/futureMap.h>
#include <tuyau/promiseMap.h>
#include <tuyau/futurePromise.h>

#include <boost/test/unit_test.hpp>

namespace ut = boost::unit_test;

const uint32_t defaultMeaningOfLife = 42;
const uint32_t defaultThanksForAllTheFish = 51;
const uint32_t addMoreFish = 10;

struct InputData
{
    explicit InputData( uint32_t value = defaultMeaningOfLife )
        : meaningOfLife( value )
    {}

    uint32_t meaningOfLife;
};

struct OutputData
{
    explicit OutputData( uint32_t value = defaultThanksForAllTheFish )
        : thanksForAllTheFish( value )
    {}

    uint32_t thanksForAllTheFish;
};

class TestFilter : public tuyau::Filter
{
    void execute( const tuyau::FutureMap& input, tuyau::PromiseMap& output ) const final
    {
        const std::vector< InputData >& results = input.get< InputData >( "TestInputData" );
        OutputData outputData;

        for( const auto& data: results )
        {
             const InputData& inputData = data;
             outputData.thanksForAllTheFish += inputData.meaningOfLife + addMoreFish;
        }

        output.set( "TestOutputData", outputData );
    }

    tuyau::DataInfos getInputDataInfos() const final
    {
        return {{ "TestInputData", tuyau::getType< InputData >( ) }};
    }

    tuyau::DataInfos getOutputDataInfos() const final
    {
        return {{ "TestOutputData", tuyau::getType< OutputData >( ) }};
    }
};

class ConvertFilter : public tuyau::Filter
{
    void execute( const tuyau::FutureMap& input, tuyau::PromiseMap& output ) const final
    {
        const std::vector< OutputData >& results = input.get< OutputData >( "ConvertInputData" );

        InputData inputData;
        for( const auto& data: results )
        {
             inputData.meaningOfLife = data.thanksForAllTheFish + addMoreFish;
        }

        output.set( "ConvertOutputData", inputData );
    }

    tuyau::DataInfos getInputDataInfos() const final
    {
        return {{ "ConvertInputData", tuyau::getType< OutputData >( ) }};

    }

    tuyau::DataInfos getOutputDataInfos() const final
    {
        return {{ "ConvertOutputData", tuyau::getType< InputData >( )}};
    }
};

bool check_error( const std::runtime_error& ) { return true; }

BOOST_AUTO_TEST_CASE( testFilterNoInput )
{
    tuyau::PipeFilterT< TestFilter > pipeFilter( "Producer" );

    // Execute will fail because there are no inputs where data is retrieved
    BOOST_CHECK_THROW( pipeFilter.execute(), std::logic_error );
    const tuyau::UniqueFutureMap portFutures( pipeFilter.getPostconditions( ));

    // Results of the filter will be empty.
    BOOST_CHECK_THROW( portFutures.get< OutputData >( "TestOutputData" ), std::runtime_error );
}

BOOST_AUTO_TEST_CASE( testFilterWithInput )
{
    tuyau::PipeFilterT< TestFilter > pipeFilter( "Producer" );
    const uint32_t inputValue = 90;

    pipeFilter.getPromise( "TestInputData" ).set( InputData( inputValue ));
    pipeFilter.execute();

    const tuyau::UniqueFutureMap portFutures( pipeFilter.getPostconditions( ));
    const OutputData& outputData =
            portFutures.get< OutputData >( "TestOutputData" );
    BOOST_CHECK_EQUAL( outputData.thanksForAllTheFish, 151 );
}

BOOST_AUTO_TEST_CASE( testSetAndGetWrongParameters )
{
    TestFilter filter;
    tuyau::PipeFilterT< TestFilter > pipeFilter( "Producer" );
    pipeFilter.getPromise( "TestInputData" ).set( InputData());

    BOOST_CHECK_THROW( pipeFilter.getPromise( "InputData" ).set( OutputData( 0 )),
                       std::runtime_error );
    pipeFilter.execute();
    const tuyau::UniqueFutureMap portFutures( pipeFilter.getPostconditions());
    BOOST_CHECK_THROW( portFutures.get<InputData>( "TestOutputData" ), std::runtime_error );
}

BOOST_AUTO_TEST_CASE( testInvalidConnection )
{
    tuyau::PipeFilterT< TestFilter > pipeFilter1( "Producer" );
    tuyau::PipeFilterT< TestFilter > pipeFilter2( "Consumer" );

    BOOST_CHECK_THROW( pipeFilter1.connect( "TestOutputData", pipeFilter2, "NotExistingConnection" ),
                       std::runtime_error );
}

BOOST_AUTO_TEST_CASE( testConnection )
{
    tuyau::PipeFilterT< TestFilter > pipeInput( "Producer" );
    tuyau::PipeFilterT< TestFilter > pipeOutput( "Consumer" );

    tuyau::PipeFilterT< ConvertFilter > convertPipeFilter( "Converter" );

    pipeInput.connect( "TestOutputData", convertPipeFilter, "ConvertInputData" );
    convertPipeFilter.connect( "ConvertOutputData", pipeOutput, "TestInputData" );

    const uint32_t inputValue = 90;
    pipeInput.getPromise( "TestInputData" ).set( InputData( inputValue ));
    pipeInput.execute();
    convertPipeFilter.execute();
    pipeOutput.execute();

    const tuyau::UniqueFutureMap portFutures( pipeOutput.getPostconditions( ));
    const OutputData& outputData =
            portFutures.get< OutputData >( "TestOutputData" );
    BOOST_CHECK_EQUAL( outputData.thanksForAllTheFish, 222 );
}

tuyau::Pipeline createPipeline( const uint32_t inputValue,
                                size_t nConvertFilter = 1 )
{
    tuyau::Pipeline pipeline;
    tuyau::PipeFilter pipeInput = pipeline.add< TestFilter >( "Producer" );
    tuyau::PipeFilter pipeOutput = pipeline.add< TestFilter >( "Consumer" );

    for( size_t i = 0; i < nConvertFilter; ++i )
    {
        std::stringstream name;
        name << "Converter" << i;
        tuyau::PipeFilter convertPipeFilter =
                pipeline.add< ConvertFilter >( name.str( ));
        pipeInput.connect( "TestOutputData", convertPipeFilter, "ConvertInputData" );
        convertPipeFilter.connect( "ConvertOutputData", pipeOutput, "TestInputData" );
    }

    pipeInput.getPromise( "TestInputData" ).set( InputData( inputValue ));
    return pipeline;
}

BOOST_AUTO_TEST_CASE( testSynchronousPipeline )
{
    const uint32_t inputValue = 90;

    tuyau::Pipeline pipeline = createPipeline( inputValue, 1 );
    pipeline.execute();

    const tuyau::Executable& pipeOutput = pipeline.getExecutable( "Consumer" );
    const tuyau::UniqueFutureMap portFutures( pipeOutput.getPostconditions( ));
    const OutputData& outputData = portFutures.get< OutputData >( "TestOutputData" );
    BOOST_CHECK_EQUAL( outputData.thanksForAllTheFish, 222 );
}

BOOST_AUTO_TEST_CASE( testWaitPipeline )
{
    const uint32_t inputValue = 90;
    tuyau::Pipeline pipeline = createPipeline( inputValue, 1 );

    tuyau::PushExecutor executor( 2 );
    const tuyau::FutureMap pipelineFutures( pipeline.schedule( executor ));
    pipelineFutures.wait();

    const tuyau::Executable& pipeOutput = pipeline.getExecutable( "Consumer" );
    const tuyau::UniqueFutureMap portFutures( pipeOutput.getPostconditions( ));
    const OutputData& outputData = portFutures.get< OutputData >( "TestOutputData" );
    BOOST_CHECK_EQUAL( outputData.thanksForAllTheFish, 222 );
}

BOOST_AUTO_TEST_CASE( testAsynchronousPipeline )
{
    const uint32_t inputValue = 90;

    tuyau::Pipeline pipeline = createPipeline( inputValue, 1 );
    tuyau::PushExecutor executor( 2 );

    pipeline.schedule( executor );
    const tuyau::Executable& pipeOutput = pipeline.getExecutable( "Consumer" );
    const tuyau::UniqueFutureMap portFutures( pipeOutput.getPostconditions( ));
    const OutputData& outputData = portFutures.get< OutputData >( "TestOutputData" );
    BOOST_CHECK_EQUAL( outputData.thanksForAllTheFish, 222 );
}

BOOST_AUTO_TEST_CASE( testOneToManyManyToOnePipeline )
{
    // Try using 1 execution thread, output result should not change
    {
        const size_t convertFilterCount = 10;
        const uint32_t inputValue = 90;

        tuyau::Pipeline pipeline = createPipeline( inputValue, convertFilterCount );

        tuyau::PushExecutor executor( 2 );
        pipeline.schedule( executor );
        const tuyau::Executable& pipeOutput = pipeline.getExecutable( "Consumer" );
        const tuyau::UniqueFutureMap portFutures( pipeOutput.getPostconditions( ));
        const OutputData& outputData = portFutures.get< OutputData >( "TestOutputData" );
        BOOST_CHECK_EQUAL( outputData.thanksForAllTheFish, 1761 );
    }

    // Try using 8 execution thread, output result should not change
    {
        const size_t convertFilterCount = 10;
        const uint32_t inputValue = 90;
        tuyau::Pipeline pipeline = createPipeline( inputValue, convertFilterCount );

        tuyau::PushExecutor executor( 8 );
        const tuyau::Futures& futures = pipeline.schedule( executor );
        const tuyau::Executable& pipeOutput = pipeline.getExecutable( "Consumer" );
        const tuyau::UniqueFutureMap portFutures1( pipeOutput.getPostconditions( ));
        const OutputData& outputData1 = portFutures1.get< OutputData >( "TestOutputData" );
        BOOST_CHECK_EQUAL( outputData1.thanksForAllTheFish, 1761 );

        // Reset the pipeline but wait pipeline execution before
        const tuyau::FutureMap futureMap( futures );
        futureMap.wait();

        pipeline.reset();
        pipeline.schedule( executor );

        tuyau::PipeFilter pipeInput =
                static_cast< const tuyau::PipeFilter& >(
                    pipeline.getExecutable( "Producer" ));
        pipeInput.getPromise( "TestInputData" ).set( InputData( inputValue ));

        const tuyau::UniqueFutureMap portFutures2( pipeOutput.getPostconditions( ));
        const OutputData& outputData2 = portFutures2.get< OutputData >( "TestOutputData" );
        BOOST_CHECK_EQUAL( outputData2.thanksForAllTheFish, 1761 );
    }
}

BOOST_AUTO_TEST_CASE( testPromiseFuture )
{
    tuyau::Promise promise( tuyau::DataInfo( "Helloworld", tuyau::getType< uint32_t >( )));
    tuyau::Future future1 = promise.getFuture();
    tuyau::Future future2 = promise.getFuture();
    BOOST_CHECK( future1 == future2 );

    // Promise only be set with the right type
    BOOST_CHECK_THROW( promise.set( 12.0f ), std::runtime_error );

    promise.set( 42u );
    BOOST_CHECK_EQUAL( future1.get< uint32_t >(), 42u );
    BOOST_CHECK_EQUAL( future2.get< uint32_t >(), 42u );

    // Promise only can be set once
    BOOST_CHECK_THROW( promise.set( 42u ), std::runtime_error );

    promise.reset();
    tuyau::Future future3 = promise.getFuture();
    BOOST_CHECK( future1 != future3 );

    // Promise is set with explicit conversion
    promise.set< uint32_t >( 43.0f );
    BOOST_CHECK_EQUAL( future1.get< uint32_t >(), 42u );
    BOOST_CHECK_EQUAL( future3.get< uint32_t >(), 43u );
}

BOOST_AUTO_TEST_CASE( testFutureMaps )
{
    tuyau::PipeFilterT< TestFilter > pipeFilter( "Producer" );
    const tuyau::Futures& uniqueFutures = pipeFilter.getPostconditions();

    // The output futures are unique
    const tuyau::UniqueFutureMap portFuturesUnique( uniqueFutures );
    const tuyau::FutureMap portFutures1( uniqueFutures );

    tuyau::Futures nonUniqueFutures = { uniqueFutures.front(), uniqueFutures.front() };

    BOOST_CHECK_THROW( const tuyau::UniqueFutureMap portFuturesNonUnique( nonUniqueFutures ),
                       std::logic_error );
    const tuyau::FutureMap portFutures2( nonUniqueFutures );
}
