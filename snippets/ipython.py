from IPython.config.loader import Config
from IPython.terminal.embed import InteractiveShellEmbed
from pykd import *

cfg = Config()

cfg.InteractiveShell.colors = 'NoColor'
cfg.PromptManager.in_template = 'In <\\#>: '
cfg.PromptManager.in2_template = '   .\\D.: '
cfg.PromptManager.out_template = 'Out<\\#>: '

ipshell = InteractiveShellEmbed(config=cfg)

ipshell()
