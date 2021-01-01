const express = require('express');
const cookieParser = require('cookie-parser');
const bodyParse = require('body-parser');
const axios = require('axios');
const log4js = require('log4js');
log4js.configure({
    appenders: { 'file': { type: 'file', filename: 'logs/log.log' } },
    categories: { default: { appenders: ['file'], level: 'debug' } }
});

const config = require('../config.json');

const serverAddr = `http://${config.server.host}:${config.server.port}`;

const logger = log4js.getLogger();


module.exports = (db) => {
    const app = express();

    app.use(cookieParser());
    app.use(bodyParse.json());

    app.post('/add', async function(req, res){
        const data = {
            data: req.body.data,
            clNum: 2
        };

        const item = await db.model.create(data);

        res.json(item.get({ plain: true }))
    })    ;

    app.get('/get', async function(req, res){
        const data = await db.model.findAll();

        res.json(data);
        logger.debug('Add');
    });

    app.get('/update', async(req, res)=>
    {
        logger.debug('restore from central data base');
        responce = await axios.get(`${serverAddr}/get`);
        await db.model.destroy({where: {}, truncate: true});
        responce.data = await  responce.data.filter(data => data.clNum === 1 && data.clNumId < 6);
        for(let i=0; i< responce.data.length; i++)
        {
            let data = responce.data[i];
            data.id = data.clNumId;
            delete data.clNumId;
            logger.debug('restore ' + data);
            await db.model.create(data);
        }

        res.statusCode = 200;
        res.json({'status': 'OK'})
    });

    app.get('/sync', async(req, res)=>
    {
        try
        {
            let response = await axios.post(`${serverAddr}/resource/`, {"id": 3, "clNum": 1});
            logger.debug(`successful sync ${responce.data}`);
            let data = response.data;
            data.id = 11;
            db.model.create(data);
        }
        catch (e)
        {
            logger.debug('not found');
        }

    });


    app.post('/set', async(req, res)=>
    {
        await db.model.destroy({where: {}, truncate: true});
        let t_data = await  req.body.filter(data => data.clNum === 1 && data.clNumId < 6);
        for(let i=0; i< t_data.length; i++)
        {
            let data = t_data[i];
            data.id = data.clNumId;
            delete data.clNumId;
            logger.debug('restore ' + data);
            await db.model.create(data);
        }

        res.statusCode = 200;
        res.json({'status': 'OK'})
    });

    app.logger = logger;
    return app;
};
