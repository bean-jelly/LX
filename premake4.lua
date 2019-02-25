solution "lx"
    projectdir = basedir()
    builddir = path.join(projectdir, '../build')
    installdir = path.join(projectdir, '../install')
    bindir = path.join(installdir, 'bin')
    libdir = path.join(installdir, 'lib')
    includedir = path.join(installdir, 'include')
    curheadersdir = includedir

    function  headersdir( dir )
        curheadersdir = dir
    end

    function headers(files)
        if type(files) == 'string' then
            files = {fies}
        end
        local finalfiles = {}
        for _, file in ipairs(files) do
            if file:find('*') then
                file = os.matchfiles(file)
            end
            finalfiles = table.join(finalfiles, file)
        end
        local source = table.concat(finalfiles, ' ')
        local target = path.join(includedir, curheadersdir)
        assert(os.mkdir(target))
        assert(os.execute('cp ' .. source .. ' ' .. target))
    end

    configurations { "Debug", "Release" }
    location(builddir)
    includedirs(includedir)
