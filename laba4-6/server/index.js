const Sequelize = require('sequelize');
const config = require('../config.json');
const db = require('./context')(Sequelize, config.mssql);
const server = require('./server')(db, config);

(async function() {
    await db.sequelize.sync({force: true});

    server.listen(config.server.port, () => console.log(`Server is running: http://${config.server.host}:${config.server.port}`));
})();