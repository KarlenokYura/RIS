const Sequelize = require('sequelize');
const config = require('../config.json');
const db = require('./context')(Sequelize, config.mssql);
const server = require('./server')(db, config);


    async function clientSim(id)
    {
        db.model.create({id: id, data: Date.now(), clNum: 1})
    }

(async function() {
    await db.sequelize.sync();

    setInterval(async () =>
    {
        db.model.count().then((count)=>{
            if(count >= 10)
            {
                server.logger.debug('trancate db');
                db.model.destroy({where: {}, truncate: true})
            }
            else
            {
                server.logger.debug('create entity');
                clientSim(count + 1);
            }
        }   );

    }, 3500);

    server.listen(config.client1.port, () => console.log('Server is running'));
})();