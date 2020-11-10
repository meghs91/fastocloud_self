FROM debian:stretch-slim

LABEL maintainer="Alexandr Topilski <support@fastogt.com>"

ENV USER fastocloud
ENV APP_NAME fastocloud
ENV PROJECT_DIR /usr/src/fastocloud
RUN groupadd -r $USER && useradd -r -g $USER $USER

COPY . $PROJECT_DIR

RUN set -ex; \
  BUILD_DEPS='ca-certificates git python3 python3-pip nano dbus'; \
  PREFIX=/usr/local; \
  apt-get update; \
  apt-get install -y $BUILD_DEPS --no-install-recommends; \
  # rm -rf /var/lib/apt/lists/*; \
  pip3 install setuptools; \
  PYFASTOGT_DIR=/usr/src/pyfastogt; \
  mkdir -p $PYFASTOGT_DIR && git clone https://github.com/fastogt/pyfastogt $PYFASTOGT_DIR && cd $PYFASTOGT_DIR && python3 setup.py install; \
  cd $PROJECT_DIR/build && ./env/build_env.py --prefix=$PREFIX --docker; \
  cd $PROJECT_DIR/build && PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig python3 build.py release $PREFIX; \
  rm -rf $PYFASTOGT_DIR $PROJECT_DIR # && apt-get purge -y --auto-remove $BUILD_DEPS

COPY docker/fastocloud.conf /etc/
RUN mkdir /var/run/$APP_NAME && chown $USER:$USER /var/run/$APP_NAME
VOLUME /var/run/$APP_NAME
WORKDIR /var/run/$APP_NAME

ENTRYPOINT ["fastocloud"]

EXPOSE 6317 8000 7000 6001
