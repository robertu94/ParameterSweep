gtest_dep = dependency('gtest', main: 'true', fallback: ['gtest', 'gtest_dep'])

test_inc = include_directories('include')

test_builder = executable('test_builder', 'test_builder.cc', include_directories: [test_inc], dependencies: [gtest_dep, parameter_sweep_dep])
test('test_builder', test_builder)
test_factor = executable('test_factor', 'test_factor.cc', include_directories: [test_inc], dependencies: [gtest_dep, parameter_sweep_dep])
test('test_factor', test_factor)
test_tuple_algorithms = executable('test_algorithms', 'test_tuple_algorithms.cc', dependencies: [gtest_dep, parameter_sweep_dep])
test('test_tuple_algorithms', test_tuple_algorithms)
