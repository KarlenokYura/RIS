module.exports = (Sequelize, config) => {
  const sequelize = new Sequelize(
    config.database,
    config.username,
    config.password,
    config.options
  );

  const Model = require('../models/model')(Sequelize, sequelize);

  return {
    model: Model,

    sequelize,
    Sequelize,
  };
};
