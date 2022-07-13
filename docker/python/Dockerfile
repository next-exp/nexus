FROM continuumio/miniconda

COPY env.yml .

RUN conda env create -f env.yml && \
  conda clean -afy
