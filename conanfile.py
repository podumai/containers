from conan import ConanFile

class Containers(ConanFile):
    settings: tuple[str] = ("os", "compiler", "build_type", "arch")
    generators: tuple[str] = ("CMakeToolchain", "CMakeDeps")

    def requirements(self) -> None:
        self.requires("catch2/3.11.0")

    def build_requirements(self) -> None:
        self.tool_requires("cmake/3.30.0")
