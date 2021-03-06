
// sharedata
/*
    当你把业务拆分到多个服务中去后，数据如何共享，可能是最易面临的问题。

    最简单粗暴的方法是通过消息传递数据。如果A服务需要B服务中的数据，可以由B服务发送一个消息，将数据打包携带过去。
    如果是一份数据，很多地方都需要获得它，那么用一个服务装下这组数据，提供一组查询接口即可。DataCenter模块对此
    做了简单的封装。

    如果你仅仅需要一组只读的结构信息分享给很多服务（比如一些配置数据），你可以把数据写到一个lua文件中，让不同的
    服务加载它。cluster的配置文件就是这样做的。注意：默认skynet使用自带的修改版lua，会缓存lua源文件。当一个lua
    文件通过loadfile加载后，磁盘上的修改不会影响下一次加载。所以你需要直接用io.open打开文件，再用load加载内存中
    的string。

    另一个更好的方法是使用sharetable模块。

    sharetable：

        skynet使用修改过的lua虚拟机，可以直接将一个table在不同的虚拟机间共享读取（但不可改变）。这个模块封装了
        这个特性。
            1.sharetable.loadfile(filename, ...)从一个源文件读取一个共享表，这个文件需要返回一个table，这个
            table可以被多个不同的服务读取。...是传给这个文件的参数。
            2.sharetable.loadstring(filename, source, ...)和loadfile类似，但是是从一个字符串读取。
            3.sharetable.loadtable(filename, tbl)直接将一个table共享。
            4.sharetable.query(filename)以filename为key查找一个被共享的table。
            5.sharetable.update(filenames)更新一个或多个key。

        注1：考虑到性能原因，推荐使用sharetable.loadfile创建这个共享表。因为使用sharetable.loadtable会经过一次
        序列化和拷贝，对于太大的表，这个过程非常耗时。

        注2：可以多次load同一个filename表，这样的话，对应的table会被更新。使用这张表的服务需要调用update更新。

        注3：一张表一旦被query一次，其数据的生命周期将一直维持调用query的该服务退出。目前没有手段主动消除对一张
        共享表的引用。

    sharedata：

        当大量的服务可能需要共享一大块并不太需要更新的结构化数据，每个服务却只使用其中一小部分。你可以设想成，
        这些数据在开发时就放在一个数据仓库中，各个服务按需要检索出需要的部分。

        整个工程需要的数据仓库可能规模庞大，每个服务却只需要使用其中一小部分数据，如果每个服务都把所有数据加载
        进内存，服务数据很多时，就因为重复加载大量不会触碰的数据而浪费大量内存。在开发期，却很难把数据切分成
        更小的粒度，因为很难时刻根据需要的变化重新划分。

        如果使用DataCenter这种中心管理方案，却无法避免每次在检索数据时都要进行一次RPC调用，性能或许无法承受。

        sharedata模块正是为了解决这种需求而设计出来的。sharedata只支持在同一节点内（同一进程下）共享数据，
        如果需要跨节点，需要自行同步处理。

            local sharedata = require "skynet.sharedata"

        可以引入这个模块。
            1.sharedata.new(name, value)在当前节点内创建一个共享数据对象。
            2.value可以是一张lua table，但不可以有环。且key必须是字符串或32bit正整数。
            3.value还可以是一段lua文本代码，而sharedata模块将解析这段代码，把它封装到一个沙盒中运行，最终取得
              它返回的table。如果它不返回table，则采用它的沙盒全局环境。
            4.sharedata.update(name, value)更新当前节点的共享数据对象。
            5.sharedata.delete(name)删除当前节点的共享数据对象。
            6.sharedata.query(name)获取当前节点的共享数据对象。

        一旦query到一个共享数据对象，你可以像普通lua那样读取其中的数据，但禁止改写。把其中的分支赋值给local变量
        是安全的，但如果你把最终的叶节点的值取出来后，就不可能被数据源的update操作更新了。所以，一般你需要持有
        至少一级表，每次用它来索引其下的数据。

        注意：query到一个对象后，除非该对象在系统中被delete，暂时没有任何手段可以清楚本地服务的代理对象。

        一旦有人调用sharedata.update，所有持有这个对象的服务都会自动去数据源头更新数据。但由于这是一个并行的过程，
        更新并不保证立刻生效。但使用共享数据的读取方一定能在同一个时间片（单次skynet消息处理过程）访问到同一版本的
        共享数据。

        更新过程是惰性的，如果你持有一个代理对象，但在更新数据后没有访问里面的数据，那么该代理会一直持有老版本的数据
        直到第一次访问。这个行为的副作用是：老版本的C对象会一直占用内存。如果你需要频繁更新数据，那么，为了加快内存
        回收，可以通知持有代理对象的服务在更新后，主动调用sharedata.flush()。

        sharedata是基于共享内存工作的，且访问共享对象内的数据并不会阻塞当前服务。所以可以保证不错的性能，并节省大量
        的内存。

        sharedata的缺点是更新一次的成本非常大，所以不适合做服务间的数据交换。你可以考虑它的替代品：stm模块。

        sharedata的实现是将原来的数据存储在自定义的table中，并在访问上保持原来的层级结构。自定义的table是一个
        userdata，其它lua VM通过自定义table的指针访问自定义table中的内容。corelib.lua中自定义了userdata的
        元表。元表中包含__index、__len、__pairs等方法，使得访问userdata像访问原有数据一样。

        如果你仅仅想利用sharedata模块分发配置表数据，而不关心自动更新数据、共享一部分内存。那么可以使用更高效的接口：

        sharedata.deepcopy(name, key, ...)这个api会获得name对应的数据表，并根据你需要的key将数据做一次深拷贝，生
        成一次lua table。这样的访问效率更高。例如：sharedata.deepcopy("foobar", "x", "y")会返回foobar.x.y的内容。

    stm：

        stm（software transactional memory）模块同样基于共享内存，所以也只能用于同一个skynet节点内。它是一个试验性
        模块，不一定比消息传递的方式更好。只是提供一个新思路来进行同一节点内的服务间数据交换。

        因为它不经过skynet的消息投递，信息投递的时效性比消息投递要好一些。但由于依旧需要在不同的lua VM间交换数据，序
        列化（使用skynet.pack和skynet.unpack）必不可少。因为绕过了消息投递，它还可以用于广播。多个读取者可以同时去读
        一个写入者更新的数据。

        stm是以一个C编写的lua模块形式提供的。

            local stm = require "skynet.stm"

        可以引入这个模块。由于api多是操作C指针，所以调用其中的api上需要小心（否则会有内存泄露）。

            1.stm.new(pointer, size)可以生成一个共享对象，生成者可以改写这个对象，pointer/size是一个C指针以及长度。
              skynet.pack可以正确生成它们。它返回一个stmobj，是一个userdata，lua的gc会正确的回收它引用的内存。
            2.stm.copy(stmobj)可以从一个共享对象中生成一份读拷贝。它返回一个stmcopy，是一个lightuserdata。通常一定
              要把这个lightuserdata传到需要读取这份数据的服务。随意丢弃这个指针会导致内存泄露。注：一个拷贝只能提供
              一个读取者使用，你不可以把这个指针传递给多服务。如果你有多个读取者，为每个人调用copy生成一份读拷贝。
            3.stm.newcopy(stmcopy)把一个C指针转换为一份读拷贝。只有经过转换，stm才能正确的管理它的声明周期。

        持有stmobj，则是这个共享对象的写入者。你可以用stmobj(pointer, size)的方式更新其中携带的信息。（这个userdata
        重载了call方法）。

        持有stmcopy，则是这个共享对象的读取者。stmcpy是用stm.copy生成的那个指针，传递给stm.newcopy构造出来的。你可以
        用stmcopy(function(pointer, size, [, ud])...end [, ud])的方式读出其中的数据。如果数据没有更新，将返回flase；
        否则，将更新过的数据指针pointer以及长度，传递给传入的反序列化函数，并返回true以及反序列化函数的结果。

        test/teststm.lua是一个简单的范例。

    sharemap：

        sharemap是对stm的简单应用。你可以用sharemap创建一个对象负责读写一张预定义的数据结构（使用sproto描述结果）。
        然后构造出读对象传递给其它服务。

        当读写方修改了数据内容后，可以通过调用commit将修改后的副本同步给所有读取方。而读取方则需要主动调用update获得
        最新副本。

        test/testsm.lua是一个简单的使用范例。
        
        注意：如果需要同步的数据结构较大，这种方式的成本也会增加。因为每次commit都会全量序列化整个结构。
*/