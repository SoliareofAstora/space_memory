import space_enviro.spaceLib as env
import numpy as np
import argparse


# https://stackoverflow.com/questions/13207678/whats-the-simplest-way-of-detecting-keyboard-input-in-python-from-the-terminal/31736883
# raw_input / input are asynchronus - good for game setup

#     py::class_< enviroment_controller >("initialize", py::init<std::string>())
#         .def("get_observations", &enviroment_controller::GetObservations)
#         .def("update", &enviroment_controller::Update);

# p = argparse.ArgumentParser()
# p.add_argument('--render_type', type=str, default='realtime')
# p.add_argument('--nHumans', type=int, default=1)
# p = p.parse_args()

b = env.initialize("habababa")
while b.bol:
    b.update(np.array([[1, 2, 3], [4, 5, 6]]))
print(b.get_observations())
print()



