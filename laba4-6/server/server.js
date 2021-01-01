const express = require('express');
const cookieParser = require('cookie-parser');
const bodyParse = require('body-parser');
const Promise = require('bluebird');
const axios = require('axios');
const log4js = require('log4js');

const config = require('../config.json');

const firstClientAddr = `http://${config.client1.host}:${config.client1.port}`;
const secondClientAddr = `http://${config.client2.host}:${config.client2.port}`;

log4js.configure({
  appenders: { file: { type: 'file', filename: 'logs/log.log' } },
  categories: { default: { appenders: ['file'], level: 'debug' } },
});
const logger = log4js.getLogger();

module.exports = (db) => {
  const app = express();

  app.use(cookieParser());
  app.use(bodyParse.json());

  app.get('/listen', async function (req, res) {
    console.log('Start fetching data every 10 seconds');

    setInterval(() => {
      console.log('fetch from 1');
      logger.debug('fetch from 1');
      axios
        .get(`${firstClientAddr}/get`)
        .then((res) => {
          logger.debug('from 1: ' + res.data.length);
          console.log('from 1: ' + res.data.length);
          replicateDB(res.data);
        })
        .catch((err) => {
          logger.debug('error 1');
          console.log('error 1');
        });
    }, 10000);

    setInterval(() => {
      logger.debug('fetch from 2');
      console.log('fetch from 2');
      axios
        .get(`${secondClientAddr}/get`)
        .then((res) => {
          logger.debug('from 2: ' + res.data.length);
          console.log('from 2: ' + res.data.length);
          replicateDB(res.data);
        })
        .catch((err) => {
          logger.debug('error 2');
          console.log('error 2');
        });
    }, 10000);

    res.sendStatus(200);
  });

  app.get('/get', async function (req, res) {
    logger.debug('send db');
    const data = await db.model.findAll();
    res.json(data);
  });

  app.get('/set', async function (req, res) {
    axios.get(`${firstClientAddr}/update`);
    axios.get(`${secondClientAddr}/update`);
    res.json({});
  });

  app.post('/resource', async (req, res) => {
    let data = req.body;
    let item = await db.model.findOne({
      where: {
        clNum: data.clNum,
        clNumId: data.id,
      },
    });
    if (item) {
      res.json({ data: item.data, clNum: item.clNum, id: item.clNumId });
    } else {
      let responce = await axios.get(
        data.clNum === 1 ? `${secondClientAddr}/get` : `${firstClientAddr}/get`
      );
      let filter_data = await responce.data.filter(
        (iter) => iter.id === data.id
      );
      if (!filter_data.length) {
        res.json({
          data: filter_data[0].data,
          clNum: filter_data[0].clNum,
          id: filter_data[0].id,
        });
      } else {
        res.statusCode = 400;
        res.json({ status: 'error' });
      }
    }
  });

  async function replicateDB(data) {
    for (let i = 0; i < data.length; i++) {
      let item = await db.model.findOne({
        where: {
          clNum: data[i].clNum,
          clNumId: data[i].id,
        },
      });

      if (item) {
        let upt = data[i];
        delete upt.id;
        await db.model.update(upt, {
          where: {
            clNum: data[i].clNum,
            clNumId: data[i].id,
          },
        });
        console.log('upd');
        logger.debug(
          'upd: ' +
            JSON.stringify({
              data: data[i].data,
              clNum: data[i].clNum,
              clNumId: data[i].id,
            })
        );
      } else {
        await db.model.create({
          data: data[i].data,
          clNum: data[i].clNum,
          clNumId: data[i].id,
        });
        logger.debug(
          'add: ' +
            JSON.stringify({
              data: data[i].data,
              clNum: data[i].clNum,
              clNumId: data[i].id,
            })
        );
        console.log('add');
      }
    }
  }

  return app;
};
