#### Yet Another Callgraph Plugin (for BinaryNinja)

![downward call graph](./.github/downward.png)

### Building + Installing

```
git clone https://github.com/cxnder/yacp.git yacp
cd yacp
git submodule update --init --recursive

# if building for stable:
# cd binja && git checkout master && cd ..

mkdir build && cd build
cmake -GNinja ../
ninja all
ninja install
```

### What does it do?

#### Incoming/Outgoing Callgraphs (or both)

See the control flow leading up to this function, or the control flow of the function itself

![](.github/upwards.png)

#### Full Program Call Graphs

![](.github/full_program.png)


### How to use it?

From the Plugins menu:

![img.png](.github/topMenu.png)

From the right-click context menu:

Right click, click "Plugins" or "Selection target", click "Callgraph", see the above same options^