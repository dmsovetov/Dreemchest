/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#include <threads/Threads.h>
#include <math.h>

using namespace dreemchest;

void taskForMainThread( thread::TaskProgress* progress, void* userData )
{
    printf( "Main: starting a main thread task...\n" );

    for( int i = 0; i < 10; i++ ) {
        printf( "Main: task progress %d\n", i );
        thread::Thread::sleep( 1000 );
    }

    printf( "Main: task completed\n" );
}

void taskBackground( thread::TaskProgress* progress, void* userData )
{
    for( int i = 0; i < 10; i++ ) {
        printf( "Background: doing a task, step %d\n", i );

        progress->setProgress( float( i ) / 10 * 100 );
        progress->setStatus( ("Background: doing a task " + std::to_string( floor( float( i ) / 10 * 100 ) ) + "%").c_str() );
        thread::Thread::sleep( 500 );
    }

    printf( "Background: task completed\n" );
}

void taskPushToMain( thread::TaskProgress* progress, void* userData )
{
    printf( "Background: starting a task\n" );
    thread::Thread::sleep( 2000 );

    printf( "Background: waiting for task to complete in main thread\n" );
    reinterpret_cast<dcTaskManager>( userData )->runMainThreadTask( dcStaticFunction( taskForMainThread ) );

    printf( "Background: task completed\n" );
}

int testThreads( int argc, char **argv )
{
    dcTaskManager taskManager = new thread::TaskManager;

    dcTaskProgressWeak progress = taskManager->runBackgroundTask( dcStaticFunction( taskBackground ) );

    while( progress != NULL ) {
        printf( "Main: %s\n", progress->status().c_str() );
        thread::Thread::sleep( 1000 );
    }

    printf( "Main: done!\n" );

    printf( "Main: now waiting for task to complete...\n" );
    progress = taskManager->runBackgroundTask( dcStaticFunction( taskBackground ) );
    progress->waitForCompletion();
    printf( "Main: task completed\n" );

    printf( "Main: testing a thread interop\n" );
    progress = taskManager->runBackgroundTask( dcStaticFunction( taskPushToMain ), taskManager );
    while( progress != NULL ) {
        taskManager->doMainThreadTasks();
        thread::Thread::sleep( 100 );
    }
    printf( "Main: done\n" );

    return 0;
}