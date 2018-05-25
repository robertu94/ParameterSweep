project('parameter_sweep', 'cpp', default_options: ['cpp_std=c++17'])

inc_dir = include_directories('include')

srcs = files(
	'src/ParameterSweep.cc'
)

parameter_sweep_lib = library(
	'ParameterSweep',
	srcs,
	include_directories: inc_dir
)

parameter_sweep_dep = declare_dependency(
	link_with: parameter_sweep_lib,
	include_directories: inc_dir
)

subdir('test')
subdir('play')
