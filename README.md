# Development of an Audio Visualization for Armatures with a Background in Music

Visualizations, regularly used in the audio production industry today, are all quite technical. They use simple mathematical transformations to display features of the signal which are not intuitive for most armatures working with. Additionally these features are normally mapped onto technical unit rather than units which come out of music.  A big group of people using audio visualizations and complex signal processing algorithms  have a background in music rather than engineering. For this group of people it can be hard to translate their trained hearing into knowledge which helps them to adjust parameters of signal processing algorithms like equalizers. The visualization of music recordings is traditionally approached from two directions. Either it has a artistic character or it is made as a technical tool. The industry is changing and what was done just by trained professionals a few years ago is now done at home by an armature with an audio interface and a mic. These armatures recording and producing music most of the time have a background in music but no technical training. To the authors knowledge there is no visualizations specifically tailored to the need of these users supporting them producing music. To the authors knowledge we are the first to specifically develop a visualization for single track recordings made for armature producers with a musical background based on a requirement study.

# Installation

Clone the repository recursively on to your linux machine:

```
git clone --recurse-submodules https://github.com/Zifzaf/Audio-Visualization-Project.git
```

Navigate to the LinuxMakefile directory in the Build folder:
```
cd Audio-Structure-Vis/Builds/LinuxMakefile/
```

Execute the make command. For this you will need to have C++ build tools installed. This may take a while. 
```
make
```

Execute the application by running:
```
./build/Audio-Visualization
```