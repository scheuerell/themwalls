import os
from setuptools import setup

setup(
    name = "walltalk",
    version = "0.0.0",
    author = "Mike Scheuerell",
    author_email = "mikescheuerell@gmail.com",
    description = "mobile UI",
    license = "BSD",
    keywords = "raspberrypi mobile ui",
    url = "http://github.com/dps/piui",
    packages=['walltalk'],
    long_description=("Add a mobile UI to your RaspberryPi"
                   " project."),
    package_data = {'walltalk' : ['walltalk/static/*']},
    classifiers=[
        "Development Status :: 1 - Alpha",
        "License :: OSI Approved :: BSD License",
    ],
    install_requires = ['cherrypy'],
)
