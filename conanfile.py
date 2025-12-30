from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ContainersDeps(ConanFile):
    settings: tuple[str] = ("os", "compiler", "build_type", "arch")
    generators: tuple[str] = ("CMakeToolchain", "CMakeDeps")

    def requirements(self) -> None:
        self.requires("catch2/3.11.0")
        self.requires("gtest/1.17.0")

    def layout(self) -> None:
        cmake_layout(self)
